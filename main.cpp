#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define BOARD_SIZE 8
#define VIRTUALARRAYLENGTH 5
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define B_KING 3
#define W_KING 4

typedef int BOOL;
#define TRUE 1
#define FALSE 0

#define MAX_BYTE 10000
#define TO_STRING(x) "%" #x "[^\n]"
#define LINE_FORMAT(l) TO_STRING(l)
#define MAX_STEP 15
#define MAX_TURN 120

#define START "START"
#define PLACE "PLACE"
#define TURN "TURN"
#define END "END"
#define WITHDRAWME "WITHDRAWME"
#define WITHDRAWOTHER "WITHDRAWOTHER"

#define KINGSCORE 20//若它是王，则一定是普兵 普兵+1 所以共+3
#define POWNSCORE 10
#define INF 999999
#define DEPTH 13

#define ATTACKWEIGHT 3
#define DEFENDWEIGHT 9
#define EATWEIGHT 1
#define DIFFERWEIGHT 5
#define REPEATWEIGHT 10
#define TIMELIMIT 1.85

#define DANGERREMAIN1 10
#define DANGERDEPTH1 13

#define DANGERREMAIN2 8
#define DANGERDEPTH2 15

#define DANGERREMAIN3 6
#define DANGERDEPTH3 19

#define DANGERREMAIN4 4
#define DANGERDEPTH4 23

#define DANGERREMAIN5 2
#define DANGERTURN 60
#define CEILINGPOINT 3
#define SAFESCORE 3
//11.18 开始项目，添加一些基本的注释


//11.28	基本完成了连续吃子的功能,已经实现了走出合法棋子。
//		已经删掉11.18的注释
//		开始着手估值函数的设计
//		增加了普通和王的初始化估值，每个普通子力+20分，每个王+50分
//		本版本的估值函数只和子力多少有关。
//		先用贪心算法。


//11.29	增加了撤回上一步command功能，并在调试时也可以用
//		基本实现了ab剪枝极大极小值搜索


//11.30	发现了可能走到无效子位的bug
//		通过标记可能变化的值来记录出错的函数位置
//		希望今天能用上位棋盘


//12.2	还没用上位棋盘呜呜呜
//		在ab剪枝中取消了用撤回操作来保留棋盘（原因：无法保留王棋）
//		改用了存储临时棋盘来储存当前棋局，在回去之后即变为刚刚的临时棋盘。
//		发现在有些时候会出bug：打印棋盘时第0排总是断断续续，一段时间之后又恢复正常
//		希望多看几篇关于算法的文章，找点设计估值函数的感觉
//		可以引入整个棋局的黑色白色子力对比，以此来减少搜索整个棋局黑白子的复杂度。
//		增加了初始化局势可吃数组的功能，方便深入进行极大极小值搜索。
//		决定抛弃助教的模板


//12.3	增加了numRemaining数组，以记录剩余子力个数
//		修改了估值函数
//		第一层估值函数就不用搜索了
//		开局第二步会送子给对手bug
//		被助教怒斥了。
//		更新了numRemaining中记载王棋的数量
//		今日困惑:如何生成足够随机的数？
//		亟待采用随机方法搜索
//		搜索中若两个估值相同则应再估值一次。
//		修复了curTurn的逻辑bug
//		王棋会在0,1和1,0之间徘徊。
//		可以写一个爬虫复盘棋局


//12.6	需要加一个debug功能
//		需要加一个判断对手落棋时间的方法以改变进攻/防卫/吃子权重
//		对于王棋在爬的情况需要改善
//		不一定只5层


//12.7	11层效果尚可。
//		吃子仍有错误，回去改bug
//		目前9层效果最好，不明为什么
//		对于可吃的子力也需要深搜！
//		用足够高的defendweight战平了sober
//		需要改进估值函数了，子力前后和左右的进攻极不平衡

//12.11	多次调参后发现9层仍是最好效果。
//		经常送子给对面吃。
//		有时不知走边路进攻底线而会在0,1 1,0处徘徊
//		程序已经基本没有bug了
//		目前最好效果：#define ATTACKWEIGHT 3
//					 #define DEFENDWEIGHT 9
// 					 #define EATWEIGHT 3
//					 #define DIFFERWEIGHT 5

//12.12	开始着手解决1,0 0,1徘徊的问题
//		需要加入随机数 随机剪枝
//		对于危险棋子优先剪枝。

//12.17	还是要用最简单的估值，别的估值效果并不好
//		需要对棋局最后几步加大估值的权重
//		从一开始就应该使用裸估，而不要考虑EATWEIGHT之类的奇技淫巧
//		还是11层效果最最好！！！！！！！
//		要用allPossibility来控制搜索层数
//		终局判断不够完善
int depth = DEPTH;
int randomint(int left, int right)
{
	int random = rand();
	int distance = right - left;
	return left + random % (right - left);
}
struct Command
{
	int x[MAX_STEP];
	int y[MAX_STEP];
	int numStep;
	int allPossibility;//用来记录一个turn所有的可能走子的
	int maxStep;//用来记录可吃子的最大步数
};
int absCal(int x)
{
	return x > 0 ? x : -x;
}
void swap(int* a, int* b)
{
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}
char curBoard1[BOARD_SIZE][BOARD_SIZE] = { 0 };//用来操作的临时棋盘
//char kingBoard[BOARD_SIZE][BOARD_SIZE] = { 0 };//记录王的位置
int curTurn;//记录现在的回合数
char board[BOARD_SIZE][BOARD_SIZE] = { 0 };//真实对局board
struct Command bestMove[MAX_TURN];//记录每手的输出
struct Command validMove[MAX_TURN][MAX_STEP];//记录每手可能的走子
int me_flag;
int other_flag;
int normalMoveDirection[4][2] = { {-1,-1},{-1,1},{1,-1},{1,1} };
int eatDirection[4][2] = { {-2,-2},{-2,2},{2,-2},{2,2} };
clock_t startTime;
clock_t endTime;
int numRemaining[5] = { 24,12,12 ,0,0 };
int virtualNumRemaining[5] = { 24,12,12,0,0 };
struct Command validEat[MAX_TURN][MAX_STEP];
int possibility;
int previousSafeDistrict[6][2] = { {1,0},{3,0},{5,0},{2,7},{4,7},{6,7} };
int mediumSafeDistrict[14][2] = { {1,0},{3,0},{5,0},{7,0},{7,2},{7,4},{7,6},{6,7},{4,7},{2,7},{0,7},{0,5},{0,3} };
void initAllStructArray()//用于初始化几个结构体数组
{
	//memset(bestMove, 0, sizeof(bestMove));
	memset(validMove, 0, sizeof(validMove));
	memset(validEat, 0, sizeof(validEat));
}

void debug(const char* str)//在任何时候输出debug信息
{
	printf("DEBUG %s\n", str);
	fflush(stdout);
}

void printBoard()//输出棋盘
{
	char visual_board[BOARD_SIZE][BOARD_SIZE + 1] = { 0 };
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i][j] == EMPTY)
			{
				visual_board[i][j] = '.';
			}
			else if (board[i][j] == BLACK)
			{
				visual_board[i][j] = 'O';
			}
			else if (board[i][j] == WHITE)
			{
				visual_board[i][j] = 'X';
			}
			else if (board[i][j] == B_KING)
			{
				visual_board[i][j] = 'B';
			}
			else if (board[i][j] == W_KING)
			{
				visual_board[i][j] = 'W';
			}
		}
		printf("%s\n", visual_board[i]);
	}
}

void printCurBoard(const char curBoard1[BOARD_SIZE][BOARD_SIZE])//输出curBoard1
{
	char visual_board[BOARD_SIZE][BOARD_SIZE + 1] = { 0 };
	for (int i = 0; i < BOARD_SIZE; i++)
		printf("-");
	printf("\n");
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (curBoard1[i][j] == EMPTY)
			{
				visual_board[i][j] = '.';
			}
			else if (curBoard1[i][j] == BLACK)
			{
				visual_board[i][j] = 'O';
			}
			else if (curBoard1[i][j] == WHITE)
			{
				visual_board[i][j] = 'X';
			}
			else if (curBoard1[i][j] == B_KING)
			{
				visual_board[i][j] = 'B';
			}
			else if (curBoard1[i][j] == W_KING)
			{
				visual_board[i][j] = 'W';
			}
		}
		printf("%s\n", visual_board[i]);
	}
	printf("↓Next Possibility↓\n");
}

BOOL isInBound(int x, int y)
{
	return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

void place(struct Command cmd, int cur_flag)
{
	int x_mid, y_mid;
	for (int i = 0; i < cmd.numStep - 1; i++)
	{
		board[cmd.x[i]][cmd.y[i]] = EMPTY;
		board[cmd.x[i + 1]][cmd.y[i + 1]] = cur_flag;
		if (abs(cmd.x[i] - cmd.x[i + 1]) == 2)
		{
			x_mid = (cmd.x[i] + cmd.x[i + 1]) / 2;
			y_mid = (cmd.y[i] + cmd.y[i + 1]) / 2;
			if (board[x_mid][y_mid] > 2)
				numRemaining[board[x_mid][y_mid]]--;
			numRemaining[3 - cur_flag]--;
			numRemaining[0]--;
			board[x_mid][y_mid] = EMPTY;
		}
	}

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		if (board[0][i] == BLACK)
		{
			board[0][i] = B_KING;
			numRemaining[B_KING]++;
		}

		if (board[BOARD_SIZE - 1][i] == WHITE)
		{
			board[BOARD_SIZE - 1][i] = W_KING;
			numRemaining[W_KING]++;
		}
	}
	curTurn++;
}

void placeCurBoard(char curBoard1[BOARD_SIZE][BOARD_SIZE], struct Command cmd, int cur_flag)//在curBoard1上走子
{
	int x_mid, y_mid;
	for (int i = 0; i < cmd.numStep - 1; i++)
	{
		curBoard1[cmd.x[i]][cmd.y[i]] = EMPTY;
		curBoard1[cmd.x[i + 1]][cmd.y[i + 1]] = cur_flag;
		if (abs(cmd.x[i] - cmd.x[i + 1]) == 2)
		{
			x_mid = (cmd.x[i] + cmd.x[i + 1]) / 2;
			y_mid = (cmd.y[i] + cmd.y[i + 1]) / 2;
			if (board[x_mid][y_mid] > 2)
				virtualNumRemaining[curBoard1[x_mid][y_mid]]--;
			virtualNumRemaining[3 - cur_flag]--;
			virtualNumRemaining[0]--;
			curBoard1[x_mid][y_mid] = EMPTY;
		}
	}
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		if (curBoard1[0][i] == BLACK)
		{
			curBoard1[0][i] = B_KING;
			virtualNumRemaining[B_KING]++;
		}
		if (board[BOARD_SIZE - 1][i] == WHITE)
		{
			curBoard1[BOARD_SIZE - 1][i] = W_KING;
			virtualNumRemaining[W_KING]++;
		}
	}
	//printCurBoard((char(*)[BOARD_SIZE])curBoard1);
}

BOOL is_empty(const char curBoard1[BOARD_SIZE][BOARD_SIZE], int x, int y)//判断是否为空
{
	if (!(x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE))
		return TRUE;
	if (curBoard1[x][y])
		return FALSE;
	return TRUE;
}

BOOL is_mine(const char curBoard1[BOARD_SIZE][BOARD_SIZE], int x, int y, int me)//判断该空是否是我的棋
{
	if (curBoard1[x][y] == me || curBoard1[x][y] == me + 2)
		return TRUE;
	return FALSE;
}

BOOL is_others(const char curBoard1[BOARD_SIZE][BOARD_SIZE], int x, int y, int others)//判断该空是否是别人的棋
{
	//int others_king = others + 2;
	if (curBoard1[x][y] == others || curBoard1[x][y] == others + 2)
		return TRUE;
	return FALSE;
}

void initAI(int me)
{
	printf("DEBUG My AI is %d,\n", me);
	fflush(stdout);
}

//-------------------------------------------------------------
//  DFS_validEat:查找吃子&多重吃子的可能性
//	scanValidEat:通过初始化查找多重吃子
//  scanValidMove:查找普通位移的可能性
//	scanAllValidMove:将上面两个函数封装,返回值为可吃子的数量,若无则返回0			
//-------------------------------------------------------------

void DFS_validEat(char curBoard1[BOARD_SIZE][BOARD_SIZE], int x, int y, int flag, int step, int turn)//用深搜查找可吃的子
{
	int otherFlag = 3 - flag;
	validEat[turn][possibility].x[step - 1] = x;
	validEat[turn][possibility].y[step - 1] = y;
	validEat[turn][possibility].numStep = step;
	for (int k = 0; k < 4; k++)
	{
		int nextX = x + normalMoveDirection[k][0];
		int nextY = y + normalMoveDirection[k][1];
		if (is_others((const char(*)[BOARD_SIZE])curBoard1, nextX, nextY, otherFlag) &&
			is_empty((const char(*)[BOARD_SIZE])curBoard1, nextX + normalMoveDirection[k][0], nextY + normalMoveDirection[k][1]) &&
			isInBound(nextX + normalMoveDirection[k][0], nextY + normalMoveDirection[k][1]))
		{//判断三个条件:此方向有子否?下一个空否?界内否?
			int temp = 0;
			temp = curBoard1[nextX][nextY];
			curBoard1[nextX][nextY] = 0;
			DFS_validEat((char(*)[BOARD_SIZE])curBoard1, nextX + normalMoveDirection[k][0], nextY + normalMoveDirection[k][1], flag, step + 1, turn);
			int stepHere = step;
			possibility++;
			for (int i = 0; i < stepHere; i++)
			{
				validEat[turn][possibility].x[i] = validEat[turn][possibility - 1].x[i];
				validEat[turn][possibility].y[i] = validEat[turn][possibility - 1].y[i];
			}
			curBoard1[nextX][nextY] = temp;
		}
	}
	for (int i = 0; i < possibility; i++)
	{
		validEat[turn][i].allPossibility = possibility - 1;
	}
}

void giveNumberMaxStep(int turn)
{
	int curPossibility = validEat[turn][0].allPossibility;
	int maxStep = 0;
	for (int i = 0; i <= curPossibility; i++)
	{
		maxStep = validEat[turn][i].numStep > maxStep ? validEat[turn][i].numStep : maxStep;
	}
	for (int i = 0; i <= curPossibility; i++)
	{
		validEat[turn][i].maxStep = maxStep;
	}
}

void scanValidEat(int flag, int turn)
{
	possibility = 0;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (curBoard1[i][j] == flag || curBoard1[i][j] == flag + 2)
			{
				DFS_validEat((char(*)[BOARD_SIZE])curBoard1, i, j, flag, 1, turn);
			}
		}
	}
}

void scanValidMove(const char curBoard1[BOARD_SIZE][BOARD_SIZE], int curFlag, int turn)
{
	int possibility = 0;
	for (int x = 0; x < BOARD_SIZE; x++)
	{
		for (int y = 0; y < BOARD_SIZE; y++)
		{
			if (curBoard1[x][y] == 2 + curFlag)//当这个棋子是这一方的王时
			{
				for (int i = 0; i < 4; i++)
				{
					int nextX = x + normalMoveDirection[i][0];
					int nextY = y + normalMoveDirection[i][1];
					if (is_empty((const char(*)[BOARD_SIZE])curBoard1, nextX, nextY) && isInBound(nextX, nextY))
					{
						validMove[turn][possibility].x[0] = x;
						validMove[turn][possibility].x[1] = nextX;
						validMove[turn][possibility].y[0] = y;
						validMove[turn][possibility].y[1] = nextY;
						validMove[turn][possibility].numStep = 2;
						possibility++;
					}
				}
			}
			else if (curBoard1[x][y] == curFlag) {//当这个棋子是这一方的普通子时
				for (int i = 2 * curFlag - 2; i < 2 * curFlag; i++)
				{
					int nextX = x + normalMoveDirection[i][0];
					int nextY = y + normalMoveDirection[i][1];
					if (is_empty((const char(*)[BOARD_SIZE])curBoard1, nextX, nextY) && isInBound(nextX, nextY))
					{
						validMove[turn][possibility].x[0] = x;
						validMove[turn][possibility].x[1] = nextX;
						validMove[turn][possibility].y[0] = y;
						validMove[turn][possibility].y[1] = nextY;
						validMove[turn][possibility].numStep = 2;
						possibility++;
					}
				}
			}
		}
	}
	for (int i = 0; i < possibility; i++)
	{
		validMove[turn][i].allPossibility = possibility - 1;
	}
}

int scanAllvalidMove(char curBoard1[BOARD_SIZE][BOARD_SIZE], int flag, int curTurn)
{
	//
	scanValidEat(flag, curTurn);
	int maxStep = 0;
	int maxIndex = 0;
	for (int i = 0; i <= possibility; i++)
	{
		if (validEat[curTurn][i].numStep > maxStep)
		{
			maxStep = validEat[curTurn][i].numStep;
			//maxIndex = i;
		}
	}
	if (maxStep >= 2)
	{
		return maxStep;
	}
	scanValidMove((const char(*)[BOARD_SIZE])curBoard1, flag, curTurn);
	return 0;
}

//-------------------------------------------------------------
//	gameOver:判断当前对局是否已经结束（用于极大极小搜索）
//	evaluation:评估函数,给当前局势打分
//	alphaBeta:剪枝函数
//	miniMax:极大极小值搜索
//-------------------------------------------------------------

int blackScore = 0, whiteScore = 0;

int gameOver(const char curBoard1[BOARD_SIZE][BOARD_SIZE], int curTurn)
//判断游戏是否结束，若结束则直接输出获胜的那方，未结束则输出0.
{
	if (virtualNumRemaining[WHITE] == 0)
		return BLACK;
	if (virtualNumRemaining[BLACK] == 0)
		return WHITE;
	blackScore = 0;
	whiteScore = 0;
	blackScore += POWNSCORE * virtualNumRemaining[BLACK] + KINGSCORE * virtualNumRemaining[B_KING];
	whiteScore += POWNSCORE * virtualNumRemaining[WHITE] + KINGSCORE * virtualNumRemaining[W_KING];
	if (curTurn >= 120)
		return blackScore > whiteScore ? BLACK : WHITE;
	return 0;
}

int validEatStep[MAX_TURN];

int evaluate(const char curBoard1[BOARD_SIZE][BOARD_SIZE], int curFlag, int turn)//估值函数，给curBoard1打分
{
	int winFlag = gameOver((char(*)[BOARD_SIZE])curBoard1, turn);
	if (winFlag) {
		if (winFlag == me_flag)
			return INF;
		else
			return -INF;
	}
	int totalScore = 0;
	/*if (turn<=DANGERTURN1)
	{
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			if (is_mine((const char(*)[BOARD_SIZE])curBoard1, 0, i, me_flag))
				myScore+
		}
	}*/
	/*if (turn >= DANGERTURN)
	{
		int k;
		k = me_flag == BLACK ? 0 : 7;
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			if (is_mine((const char(*)[BOARD_SIZE])curBoard1,k,i,me_flag))
				totalScore+=CEILINGPOINT;

		}
	}*/
	if (turn <= 105)
	{
		for (int i = 0; i < 6; i++)
		{
			if (is_mine((const char(*)[BOARD_SIZE])curBoard1, previousSafeDistrict[i][0], previousSafeDistrict[i][1], me_flag))
			{
				totalScore += SAFESCORE;
			}
			if (is_others((const char(*)[BOARD_SIZE])curBoard1, previousSafeDistrict[i][0], previousSafeDistrict[i][1], other_flag))
			{
				totalScore -= SAFESCORE;
			}
		}
	}
	/*else if (turn <= 80)
	{
		for (int i = 0; i < 14; i++)
		{
			if (is_mine((const char(*)[BOARD_SIZE])curBoard1, mediumSafeDistrict[i][0], previousSafeDistrict[i][1], me_flag))
			{
				totalScore += SAFESCORE;
			}
			if (is_others((const char(*)[BOARD_SIZE])curBoard1, mediumSafeDistrict[i][0], previousSafeDistrict[i][1], other_flag))
			{
				totalScore -= SAFESCORE;
			}
		}
	}*/
	int differnum = blackScore - whiteScore;
	totalScore += me_flag == BLACK ? differnum : -differnum;
	return totalScore;
}
//int nodes;

void giveBoard(char originBoard[BOARD_SIZE][BOARD_SIZE], char targetBoard[BOARD_SIZE][BOARD_SIZE])
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			targetBoard[i][j] = originBoard[i][j];
		}
	}
}

void giveNumRemaining(int originArray[3], int targetArray[3], int length)
{
	for (int i = 0; i < length; i++)
	{
		targetArray[i] = originArray[i];
	}
}
int flag_hold;
void createRandomArray(int* array, int length)//生成一个随机数组
{
	srand((unsigned)time(NULL));
	for (int i = 0; i < length; i++)
	{
		array[i] = i;//生成一个自然数列
	}
	for (int i = 0; i < length; i++)
	{
		int temp = rand() % (length - i) + i;
		swap(&array[temp], &array[i]);
	}
}
int alphaBeta(char curBoard1[BOARD_SIZE][BOARD_SIZE], int turn, int depth, int flag, int alpha, int beta)//alphabeta剪枝
{
	int myScore = evaluate((const char(*)[BOARD_SIZE])curBoard1, flag, turn);
	/*printCurBoard((char(*)[BOARD_SIZE])curBoard1);*/
	endTime = clock();
	double runTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
	if ((runTime) > TIMELIMIT) {
		if (!flag_hold) {
			printf("DEBUG Time out ,cut!!numremaining%d depth%d\n", virtualNumRemaining[me_flag], depth);
			flag_hold = 1;
		}
		return myScore;
	}
	if (depth <= 0 || abs(myScore) > 90000)
	{
		return myScore;
	}
	validEatStep[turn] = scanAllvalidMove((char(*)[BOARD_SIZE])curBoard1, flag, turn);
	int curValidEatStep = validEatStep[turn];
	//进入ab剪枝
	if (flag == me_flag)
	{
		if (validEatStep[turn])//有吃必吃
		{
			giveNumberMaxStep(turn);
			for (int i = 0; i <= validEat[turn][0].allPossibility; i++)
			{
				if (validEat[turn][i].numStep == validEat[turn][i].maxStep)
				{
					int tempNumRemaining[5] = { 0 };
					giveNumRemaining(virtualNumRemaining, tempNumRemaining, VIRTUALARRAYLENGTH);
					char tmpBoard[8][8] = { 0 };//临时棋盘，用于恢复操作之前的棋盘，下面的临时棋盘同上
					giveBoard((char(*)[BOARD_SIZE])curBoard1, (char(*)[BOARD_SIZE])tmpBoard);
					placeCurBoard((char(*)[BOARD_SIZE])curBoard1, validEat[turn][i], flag);
					int value = alphaBeta((char(*)[BOARD_SIZE])curBoard1, turn + 1, depth - 1, other_flag, alpha, beta);
					giveBoard((char(*)[BOARD_SIZE])tmpBoard, (char(*)[BOARD_SIZE])curBoard1);//恢复之前的棋盘。
					giveNumRemaining(tempNumRemaining, virtualNumRemaining, VIRTUALARRAYLENGTH);
					if (value > alpha)
					{
						alpha = value;
						//printf("current alpha=%d\n", alpha);
						bestMove[turn] = validEat[turn][i];
					}
					if (alpha >= beta)
						break;
				}
			}
			return alpha;
		}
		else
		{
			int randomArray[MAX_STEP];
			createRandomArray(randomArray, validMove[turn][0].allPossibility);
			for (int i = 0; i <= validMove[turn][0].allPossibility; i++)
			{
				int tempNumRemaining[5] = { 0 };
				giveNumRemaining(virtualNumRemaining, tempNumRemaining, VIRTUALARRAYLENGTH);
				char tmpBoard[8][8] = { 0 };
				giveBoard((char(*)[BOARD_SIZE])curBoard1, (char(*)[BOARD_SIZE])tmpBoard);
				placeCurBoard((char(*)[BOARD_SIZE])curBoard1, validMove[turn][randomArray[i]], flag);
				int value = alphaBeta((char(*)[BOARD_SIZE])curBoard1, turn + 1, depth - 1, other_flag, alpha, beta);
				giveBoard((char(*)[BOARD_SIZE])tmpBoard, (char(*)[BOARD_SIZE])curBoard1);
				giveNumRemaining(tempNumRemaining, virtualNumRemaining, VIRTUALARRAYLENGTH);
				if (value > alpha)
				{
					alpha = value;
					//printf("current alpha=%d\n", alpha);
					bestMove[turn] = validMove[turn][i];
				}
				if (alpha >= beta)
				{
					//printf("alpha>=beta,NOW CUT THE BRANCH.");
					break;
				}
			}
			return alpha;
		}
	}
	else
	{
		if (validEatStep[turn])//有吃必吃
		{
			giveNumberMaxStep(turn);
			for (int i = 0; i <= validEat[turn][0].allPossibility; i++)
			{
				if (scanAllvalidMove((char(*)[BOARD_SIZE])curBoard1, flag, turn))
				{
					if (validEat[turn][i].numStep == validEat[turn][i].maxStep)
					{
						int tempNumRemaining[5] = { 0 };
						giveNumRemaining(virtualNumRemaining, tempNumRemaining, VIRTUALARRAYLENGTH);
						char tmpBoard[8][8] = { 0 };
						giveBoard((char(*)[BOARD_SIZE])curBoard1, (char(*)[BOARD_SIZE])tmpBoard);
						placeCurBoard((char(*)[BOARD_SIZE])curBoard1, validEat[turn][i], flag);
						int value = alphaBeta((char(*)[BOARD_SIZE])curBoard1, turn + 1, depth - 1, me_flag, alpha, beta);
						giveBoard((char(*)[BOARD_SIZE])tmpBoard, (char(*)[BOARD_SIZE])curBoard1);
						giveNumRemaining(tempNumRemaining, virtualNumRemaining, VIRTUALARRAYLENGTH);
						if (value < beta)
						{
							beta = value;
							//printf("current beta=%d\n", beta);
							bestMove[turn] = validEat[turn][i];
						}
						if (alpha >= beta)
						{
							//printf("alpha>=beta,NOW CUT THE BRANCH.");
							break;
						}
					}
				}
			}
			return beta;
		}
		else
		{
			int randomArray[MAX_STEP];
			createRandomArray(randomArray, validMove[turn][0].allPossibility);
			for (int i = 0; i <= validMove[turn][0].allPossibility; i++)
			{
				int tempNumRemaining[5] = { 0 };
				giveNumRemaining(virtualNumRemaining, tempNumRemaining, VIRTUALARRAYLENGTH);
				char tmpBoard[8][8] = { 0 };
				giveBoard((char(*)[BOARD_SIZE])curBoard1, (char(*)[BOARD_SIZE])tmpBoard);
				placeCurBoard((char(*)[BOARD_SIZE])curBoard1, validMove[turn][randomArray[i]], flag);
				int value = alphaBeta((char(*)[BOARD_SIZE])curBoard1, turn + 1, depth - 1, me_flag, alpha, beta);
				giveBoard((char(*)[BOARD_SIZE])tmpBoard, (char(*)[BOARD_SIZE])curBoard1);
				giveNumRemaining(tempNumRemaining, virtualNumRemaining, VIRTUALARRAYLENGTH);
				if (value < beta)
				{
					beta = value;
					//printf("current beta=%d\n", beta);
					bestMove[turn] = validMove[turn][i];
				}
				if (alpha >= beta)
				{
					//printf("alpha>=beta,NOW CUT THE BRANCH.");
					break;
				}
			}
			return beta;
		}
	}
}

int nodesMax;

int miniMax(const char curBoard1[BOARD_SIZE][BOARD_SIZE], int depth)//极大极小值搜索
{
	flag_hold = 0;
	int x = alphaBeta((char(*)[BOARD_SIZE])curBoard1, curTurn, depth, me_flag, -INF, INF);
	int curEvaluation = evaluate((char(*)[BOARD_SIZE])curBoard1, me_flag, curTurn);
	printf("DEBUG Current node result is %d\n", curEvaluation);
	fflush(stdout);
	return x;
}

int thisNodeValue = 0;

struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me)
{
	initAllStructArray();
	memset(bestMove, 0, sizeof(bestMove));
	if (thisNodeValue < 0)//根据当前子力多少来决定搜索深度
		if (numRemaining[me_flag] <= DANGERREMAIN1)
		{
			depth = DANGERDEPTH1;
		}
		else if (numRemaining[me_flag] <= DANGERREMAIN2)
		{
			depth = DANGERDEPTH2;
		}
		else if (numRemaining[me_flag] <= DANGERREMAIN3)
		{
			depth = DANGERDEPTH3;
		}
		else if (numRemaining[me_flag] <= DANGERREMAIN4)
		{
			depth = DANGERDEPTH4;
		}
		else if (numRemaining[me_flag] <= DANGERREMAIN5)
		{
			depth = (MAX_TURN - curTurn) / 2 + 1;
		}
	thisNodeValue = miniMax((const char(*)[BOARD_SIZE])curBoard1, depth);
	if (bestMove[curTurn].numStep < 2)
	{
		scanAllvalidMove((char(*)[BOARD_SIZE])curBoard1, me, curTurn);
		int randMove = randomint(0, validMove[curTurn][0].allPossibility);//
		bestMove[curTurn] = validMove[curTurn][randMove];
	}
	return bestMove[curTurn];
}
void start(int flag)
{
	memset(board, 0, sizeof(board));
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 8; j += 2)
		{
			board[i][j + (i + 1) % 2] = WHITE;
		}
	}
	for (int i = 5; i < 8; i++)
	{
		for (int j = 0; j < 8; j += 2)
		{
			board[i][j + (i + 1) % 2] = BLACK;
		}
	}
	initAI(flag);
}

void turn()
{
	startTime = clock();
	giveBoard((char(*)[BOARD_SIZE])board, (char(*)[BOARD_SIZE])curBoard1);//将真实的棋盘变为虚拟棋盘,在虚拟棋盘上操作。
	giveNumRemaining(numRemaining, virtualNumRemaining, VIRTUALARRAYLENGTH);
	struct Command command = aiTurn((const char(*)[BOARD_SIZE])board, me_flag);
	place(command, me_flag);
	printf("%d", command.numStep);
	for (int i = 0; i < command.numStep; i++)
	{
		printf(" %d,%d", command.x[i], command.y[i]);
	}
	printf("\n");
	endTime = clock();
	double runTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
	printf("DEBUG %f\n", runTime);
	fflush(stdout);
}

void end(int x)
{

}

void loop()
{
	char tag[10] = { 0 };
	char buffer[MAX_BYTE + 1] = { 0 };
	struct Command command = {
			.x = {0},
			.y = {0},
			.numStep = 0 };
	int status;
	while (TRUE)
	{
		memset(tag, 0, sizeof(tag));
		memset(buffer, 0, sizeof(buffer));
		scanf("%s", tag);
		if (strcmp(tag, START) == 0)
		{
			scanf("%d", &me_flag);
			other_flag = 3 - me_flag;
			start(me_flag);
			printf("OK\n");
			fflush(stdout);
		}
		else if (strcmp(tag, PLACE) == 0)
		{
			scanf("%d", &command.numStep);
			for (int i = 0; i < command.numStep; i++)
			{
				scanf("%d,%d", &command.x[i], &command.y[i]);
			}
			bestMove[curTurn] = command;
			place(command, other_flag);
		}
		else if (strcmp(tag, TURN) == 0)
		{
			turn();
		}
		else if (strcmp(tag, END) == 0)
		{
			scanf("%d", &status);
			end(status);
		}
		//printBoard();
	}
}

int main(int argc, char* argv[])
{
	loop();
	return 0;
}