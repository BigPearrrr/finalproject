#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define BOARD_SIZE 8
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
#define MAX_TURN 60

#define START "START"
#define PLACE "PLACE"
#define TURN "TURN"
#define END "END"

//11.18 开始项目，添加一些基本的注释


//11.28	基本完成了连续吃子的功能,已经实现了走出合法棋子。
//		已经删掉11.18的注释
//		开始着手估值函数的设计
//		增加了普通和王的初始化估值，每个普通子力+20分，每个王+50分
//		本版本的估值函数只和子力多少有关。
//		先用贪心算法。
struct Command
{
	int x[MAX_STEP];
	int y[MAX_STEP];
	int numStep;
};
char curBoard[BOARD_SIZE][BOARD_SIZE] = { 0 };//用来操作的临时棋盘
int curTurn;//记录现在的回合数
char board[BOARD_SIZE][BOARD_SIZE] = { 0 };
struct Command bestMove[MAX_TURN];
struct Command validMove[MAX_TURN][MAX_STEP];
int me_flag;
int other_flag;
clock_t startTime;
clock_t endTime;
void debug(const char* str)//using it to throw a debug message
{
	printf("DEBUG %s\n", str);
	fflush(stdout);
}
void printBoard()
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
		}
		printf("%s\n", visual_board[i]);
	}
}
void printCurBoard(const char curBoard[BOARD_SIZE][BOARD_SIZE])
{
	char visual_board[BOARD_SIZE][BOARD_SIZE + 1] = { 0 };
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (curBoard[i][j] == EMPTY)
			{
				visual_board[i][j] = '.';
			}
			else if (curBoard[i][j] == BLACK)
			{
				visual_board[i][j] = 'O';
			}
			else if (curBoard[i][j] == WHITE)
			{
				visual_board[i][j] = 'X';
			}
		}
		printf("%s\n", visual_board[i]);
	}
	for (int i = 0; i < BOARD_SIZE; i++)
		printf("-");
	printf("\n");
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
			board[x_mid][y_mid] = EMPTY;
		}
	}
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		if (board[0][i] == BLACK)
			board[0][i] = B_KING;
		if (board[BOARD_SIZE - 1][i] == WHITE)
			board[BOARD_SIZE - 1][i] = W_KING;
	}
	curTurn++;
}
void placeCurBoard(char curBoard[BOARD_SIZE][BOARD_SIZE],struct Command cmd, int cur_flag)
{
	int x_mid, y_mid;
	for (int i = 0; i < cmd.numStep - 1; i++)
	{
		curBoard[cmd.x[i]][cmd.y[i]] = EMPTY;
		curBoard[cmd.x[i + 1]][cmd.y[i + 1]] = cur_flag;
		if (abs(cmd.x[i] - cmd.x[i + 1]) == 2)
		{
			x_mid = (cmd.x[i] + cmd.x[i + 1]) / 2;
			y_mid = (cmd.y[i] + cmd.y[i + 1]) / 2;
			curBoard[x_mid][y_mid] = EMPTY;
		}
	}
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		if (curBoard[0][i] == BLACK)
			curBoard[0][i] = B_KING;
		if (curBoard[BOARD_SIZE - 1][i] == WHITE)
			curBoard[BOARD_SIZE - 1][i] = W_KING;
	}
	printCurBoard((char(*)[BOARD_SIZE])curBoard);
}
int direction[4][2] = { {-1,-1},{-1,1},{1,-1},{1,1} };
BOOL is_empty(const char curBoard[BOARD_SIZE][BOARD_SIZE],int x, int y)
{
	if (!(x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE))
		return TRUE;
	if (curBoard[x][y])
		return FALSE;
	return TRUE;
}
BOOL is_mine(const char curBoard[BOARD_SIZE][BOARD_SIZE],int x, int y, int me)
{
	int my_king;
	my_king = me + 2;
	if (curBoard[x][y] == me || curBoard[x][y] == my_king)
		return TRUE;
	return FALSE;
}
BOOL is_others(const char curBoard[BOARD_SIZE][BOARD_SIZE],int x, int y, int others)
{
	//int others_king = others + 2;
	if (curBoard[x][y] == others || curBoard[x][y] == others + 2)
		return TRUE;
	return FALSE;
}
void initAI(int me)
{

}
//-------------------------------------------------------------
//
//  DFS_validEat:查找多重吃子的可能性
//	scanValidEat:通过初始化查找多重吃子
//  scanValidMove:查找普通位移的可能性
//-------------------------------------------------------------
char nextX, nextY;
struct Command validEat[MAX_TURN][MAX_STEP];
int possibility;
void DFS_validEat(char curBoard[BOARD_SIZE][BOARD_SIZE],int x, int y, int flag, int step, int turn)//用深搜查找可吃的子
{
	int otherFlag = 3 - flag;
	validEat[turn][possibility].x[step - 1] = x;
	validEat[turn][possibility].y[step - 1] = y;
	validEat[turn][possibility].numStep = step;
	for (int k = 0; k < 4; k++)
	{
		nextX = x + direction[k][0];
		nextY = y + direction[k][1];
		if (is_others((const char(*)[BOARD_SIZE])curBoard,nextX, nextY, otherFlag) &&
			is_empty((const char(*)[BOARD_SIZE])curBoard,nextX + direction[k][0], nextY + direction[k][1]) &&
			isInBound(nextX + direction[k][0], nextY + direction[k][1]))
		{//判断三个条件:此方向有子否?空否?界内否?
			int temp = 0;
			temp = curBoard[nextX][nextY];
			curBoard[nextX][nextY] = 0;
			DFS_validEat(( char(*)[BOARD_SIZE])curBoard,nextX + direction[k][0], nextY + direction[k][1], flag, step + 1, turn);
			int stepHere = step;
			possibility++;
			for (int i = 0; i < stepHere; i++)
			{
				validEat[turn][possibility].x[i] = validEat[turn][possibility - 1].x[i];
				validEat[turn][possibility].y[i] = validEat[turn][possibility - 1].y[i];
			}
			curBoard[nextX][nextY] = temp;
		}
	}
}

void scanValidEat(int flag, int turn)
{
	possibility = 0;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (curBoard[i][j] == flag)
			{
				DFS_validEat((char(*)[BOARD_SIZE])curBoard,i, j, flag, 1, turn);
			}
		}
	}
}

void scanValidMove(const char curBoard[BOARD_SIZE][BOARD_SIZE],int curFlag, int turn)
{
	possibility = 0;
	for (int x = 0; x < BOARD_SIZE; x++)
	{
		for (int y = 0; y < BOARD_SIZE; y++)
		{
			if (curBoard[x][y] == 2 + curFlag)//当这个棋子是这一方的王时
			{
				for (int i = 0; i < 4; i++)
				{
					nextX = x + direction[i][0];
					nextY = y + direction[i][1];
					if (is_empty((const char(*)[BOARD_SIZE])curBoard,nextX, nextY) && isInBound(nextX, nextY))
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
			else if (curBoard[x][y] == curFlag) {//当这个棋子是这一方的普通子时
				for (int i = 2 * curFlag - 2; i < 2 * curFlag; i++)
				{
					nextX = x + direction[i][0];
					nextY = y + direction[i][1];
					if (is_empty((const char(*)[BOARD_SIZE])curBoard,nextX, nextY) && isInBound(nextX, nextY))
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
}

int scanAllvalidMove(int flag, int curTurn)
{
	scanValidEat(flag, curTurn);
	int maxStep = 0;
	int maxIndex = 0;
	for (int i = 0; i <= possibility; i++)
	{
		if (validEat[curTurn][i].numStep > maxStep)
		{
			maxStep = validEat[curTurn][i].numStep;
			maxIndex = i;
		}
	}
	if (maxStep >= 2)
	{
		bestMove[curTurn] = validEat[curTurn][maxIndex];
		return 1;
	}
	scanValidMove((const char(*)[BOARD_SIZE])curBoard,flag, curTurn);
	return 0;
}

//-------------------------------------------------------------
//evaluation:评估函数,给当前局势打分
//-------------------------------------------------------------
#define KINGSCORE 50
#define POWNSCORE 20
//#define INFINITY 99999
int gameOver(const char curBoard[BOARD_SIZE][BOARD_SIZE],int curTurn)
{
	int blackScore=0, whiteScore=0;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			switch (curBoard[i][j])
			{
			case 1:
			{
				blackScore++;
				break;
			}
			case 2:
			{
				whiteScore++;
				break;
			}
			case 3:
			{
				blackScore += 3;
				break;
			}
			case 4:
			{
				blackScore += 3;
				break;
			}
			}
		}
	}
	if (curTurn >= 60)
	{
		return blackScore > whiteScore ? BLACK : WHITE;
	}
	if (blackScore == 0)
	{
		return WHITE;
	}
	if (whiteScore == 0)
	{
		return BLACK;
	}
	return 0;
}
int evaluate(const char curBoard[BOARD_SIZE][BOARD_SIZE],int turn)
{
	int winFlag = gameOver((const char(*)[BOARD_SIZE])curBoard, curTurn);
	int blackScore=0;
	if (winFlag)
	{
		if (winFlag == me_flag)
			return INFINITY;
		else
			return -INFINITY;
	}
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			switch (curBoard[i][j])
			{
			case 1:
			{
				blackScore += POWNSCORE;
				break;
			}
			case 2:
			{
				blackScore -= POWNSCORE;
				break;
			}
			case 3:
			{
				blackScore += KINGSCORE;
				break;
			}
			case 4:
			{
				blackScore -= KINGSCORE;
				break;
			}
			}
		}
	}
	if (me_flag == BLACK)
		return blackScore;
	else
		return -blackScore;
}
int nodes;

int alphaBeta(char curBoard[BOARD_SIZE][BOARD_SIZE], int turn, int depth, int flag, int alpha, int beta)//alphabeta剪枝
{
	//nodes++;
	int myScore = evaluate((const char(*)[BOARD_SIZE])curBoard, turn);
	if (depth == 0 || abs(myScore) > 90000)
	{
		return myScore;
	}
	if (flag == me_flag)
	{//进入ab剪枝
		if (scanAllvalidMove(flag, turn))//如果已经可吃子的话那么bestMove一定就是吃子这一步
		{
			placeCurBoard((char(*)[BOARD_SIZE])curBoard,bestMove[turn], flag);
			int value = alphaBeta(( char(*)[BOARD_SIZE])curBoard, turn+1, depth - 1, other_flag, alpha, beta);
			if (value > alpha)
			{
				alpha = value;
			}
			return alpha;
		}
		else
		{
			//int validNum = sizeof(validMove[turn]) / sizeof(validMove[turn][0]);
			for (int i = 0; i < possibility; i++)
			{
				placeCurBoard((char(*)[BOARD_SIZE])curBoard,validMove[turn][i], flag);
				int value = alphaBeta(( char(*)[BOARD_SIZE])curBoard, turn+1, depth - 1, other_flag, alpha, beta);
				if (value > alpha)
				{
					alpha = value;
					bestMove[turn] = validMove[turn][i];
				}
				if (alpha >= beta)
					break;
			}
			return alpha;
		}
	}
	else
	{
		if (scanAllvalidMove(flag, turn))
		{
			placeCurBoard((char(*)[BOARD_SIZE])curBoard,bestMove[turn], flag);
			int value = alphaBeta(( char(*)[BOARD_SIZE])curBoard, turn+1, depth - 1, me_flag, alpha, beta);
			if (value < beta)
			{
				beta = value;
			}
			return alpha;
		}
		else
		{
			for (int i = 0; i <= possibility; i++)
			{
				placeCurBoard((char(*)[BOARD_SIZE])curBoard,validMove[turn][i], flag);
				int value = alphaBeta((char(*)[BOARD_SIZE])curBoard, turn+1, depth - 1, me_flag, alpha, beta);
				if (value <beta)
				{
					beta = value;
					bestMove[turn] = validMove[turn][i];
				}
				if (alpha >= beta)
					break;
			}
			return beta;
		}
	}
}
int nodesMax;
int miniMax(const char curBoard[BOARD_SIZE][BOARD_SIZE], int depth)
{
	//nodesMax = pow(MAX_STEP, depth - 2);
	//nodes = 0;
	int x = alphaBeta(( char(*)[BOARD_SIZE])curBoard, curTurn,depth, me_flag, -INFINITY, INFINITY);
	return x;
}
struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me)
{
	miniMax((const char(*)[BOARD_SIZE])curBoard, 4);
	return bestMove[curTurn];
	//evaluate((const char(*)[BOARD_SIZE])board,curTurn);
	/*scanValidEat(me, curTurn);
	startTime = clock();
	int maxStep = 0;
	int maxIndex = 0;
	for (int i = 0; i <= possibility; i++)
	{
		if (validEat[curTurn][i].numStep > maxStep) 
		{
			maxStep = validEat[curTurn][i].numStep;
			maxIndex = i;
		}
	}
	if (maxStep >= 2)
	{
		return validEat[curTurn][maxIndex];
	}
	else 
	{
		scanValidMove(me, curTurn);
		return validMove[curTurn][0];
	}*/
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
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			curBoard[i][j] = board[i][j];
		}
	}
	struct Command command = aiTurn((const char(*)[BOARD_SIZE])board, me_flag);
	place(command, me_flag);
	printf("%d", command.numStep);
	for (int i = 0; i < command.numStep; i++)
	{
		printf(" %d,%d", command.x[i], command.y[i]);
	}
	printf("\n%f\n", (float)startTime - (float)endTime);
	fflush(stdout);
}

void end(int x)
{

}

void loop()
{
	//freopen("../input", "r", stdin);
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
	}
}

int main(int argc, char* argv[])
{
	loop();
	return 0;
}