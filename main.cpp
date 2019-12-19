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

#define KINGSCORE 20//������������һ�����ձ� �ձ�+1 ���Թ�+3
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
//11.18 ��ʼ��Ŀ�����һЩ������ע��


//11.28	����������������ӵĹ���,�Ѿ�ʵ�����߳��Ϸ����ӡ�
//		�Ѿ�ɾ��11.18��ע��
//		��ʼ���ֹ�ֵ���������
//		��������ͨ�����ĳ�ʼ����ֵ��ÿ����ͨ����+20�֣�ÿ����+50��
//		���汾�Ĺ�ֵ����ֻ�����������йء�
//		����̰���㷨��


//11.29	�����˳�����һ��command���ܣ����ڵ���ʱҲ������
//		����ʵ����ab��֦����Сֵ����


//11.30	�����˿����ߵ���Ч��λ��bug
//		ͨ����ǿ��ܱ仯��ֵ����¼����ĺ���λ��
//		ϣ������������λ����


//12.2	��û����λ����������
//		��ab��֦��ȡ�����ó��ز������������̣�ԭ���޷��������壩
//		�����˴洢��ʱ���������浱ǰ��֣��ڻ�ȥ֮�󼴱�Ϊ�ոյ���ʱ���̡�
//		��������Щʱ����bug����ӡ����ʱ��0�����Ƕ϶�������һ��ʱ��֮���ָֻ�����
//		ϣ���࿴��ƪ�����㷨�����£��ҵ���ƹ�ֵ�����ĸо�
//		��������������ֵĺ�ɫ��ɫ�����Աȣ��Դ�����������������ֺڰ��ӵĸ��Ӷȡ�
//		�����˳�ʼ�����ƿɳ�����Ĺ��ܣ�����������м���Сֵ������
//		�����������̵�ģ��


//12.3	������numRemaining���飬�Լ�¼ʣ����������
//		�޸��˹�ֵ����
//		��һ���ֵ�����Ͳ���������
//		���ֵڶ��������Ӹ�����bug
//		������ŭ���ˡ�
//		������numRemaining�м������������
//		��������:��������㹻���������
//		ؽ�����������������
//		��������������ֵ��ͬ��Ӧ�ٹ�ֵһ�Ρ�
//		�޸���curTurn���߼�bug
//		�������0,1��1,0֮���ǻ���
//		����дһ�����渴�����


//12.6	��Ҫ��һ��debug����
//		��Ҫ��һ���ж϶�������ʱ��ķ����Ըı����/����/����Ȩ��
//		�������������������Ҫ����
//		��һ��ֻ5��


//12.7	11��Ч���пɡ�
//		�������д��󣬻�ȥ��bug
//		Ŀǰ9��Ч����ã�����Ϊʲô
//		���ڿɳԵ�����Ҳ��Ҫ���ѣ�
//		���㹻�ߵ�defendweightսƽ��sober
//		��Ҫ�Ľ���ֵ�����ˣ�����ǰ������ҵĽ�������ƽ��

//12.11	��ε��κ���9���������Ч����
//		�������Ӹ�����ԡ�
//		��ʱ��֪�߱�·�������߶�����0,1 1,0���ǻ�
//		�����Ѿ�����û��bug��
//		Ŀǰ���Ч����#define ATTACKWEIGHT 3
//					 #define DEFENDWEIGHT 9
// 					 #define EATWEIGHT 3
//					 #define DIFFERWEIGHT 5

//12.12	��ʼ���ֽ��1,0 0,1�ǻ�������
//		��Ҫ��������� �����֦
//		����Σ���������ȼ�֦��

//12.17	����Ҫ����򵥵Ĺ�ֵ����Ĺ�ֵЧ��������
//		��Ҫ�������󼸲��Ӵ��ֵ��Ȩ��
//		��һ��ʼ��Ӧ��ʹ�����������Ҫ����EATWEIGHT֮����漼����
//		����11��Ч������ã�������������
//		Ҫ��allPossibility��������������
//		�վ��жϲ�������
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
	int allPossibility;//������¼һ��turn���еĿ������ӵ�
	int maxStep;//������¼�ɳ��ӵ������
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
char curBoard1[BOARD_SIZE][BOARD_SIZE] = { 0 };//������������ʱ����
//char kingBoard[BOARD_SIZE][BOARD_SIZE] = { 0 };//��¼����λ��
int curTurn;//��¼���ڵĻغ���
char board[BOARD_SIZE][BOARD_SIZE] = { 0 };//��ʵ�Ծ�board
struct Command bestMove[MAX_TURN];//��¼ÿ�ֵ����
struct Command validMove[MAX_TURN][MAX_STEP];//��¼ÿ�ֿ��ܵ�����
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
void initAllStructArray()//���ڳ�ʼ�������ṹ������
{
	//memset(bestMove, 0, sizeof(bestMove));
	memset(validMove, 0, sizeof(validMove));
	memset(validEat, 0, sizeof(validEat));
}

void debug(const char* str)//���κ�ʱ�����debug��Ϣ
{
	printf("DEBUG %s\n", str);
	fflush(stdout);
}

void printBoard()//�������
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

void printCurBoard(const char curBoard1[BOARD_SIZE][BOARD_SIZE])//���curBoard1
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
	printf("��Next Possibility��\n");
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

void placeCurBoard(char curBoard1[BOARD_SIZE][BOARD_SIZE], struct Command cmd, int cur_flag)//��curBoard1������
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

BOOL is_empty(const char curBoard1[BOARD_SIZE][BOARD_SIZE], int x, int y)//�ж��Ƿ�Ϊ��
{
	if (!(x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE))
		return TRUE;
	if (curBoard1[x][y])
		return FALSE;
	return TRUE;
}

BOOL is_mine(const char curBoard1[BOARD_SIZE][BOARD_SIZE], int x, int y, int me)//�жϸÿ��Ƿ����ҵ���
{
	if (curBoard1[x][y] == me || curBoard1[x][y] == me + 2)
		return TRUE;
	return FALSE;
}

BOOL is_others(const char curBoard1[BOARD_SIZE][BOARD_SIZE], int x, int y, int others)//�жϸÿ��Ƿ��Ǳ��˵���
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
//  DFS_validEat:���ҳ���&���س��ӵĿ�����
//	scanValidEat:ͨ����ʼ�����Ҷ��س���
//  scanValidMove:������ͨλ�ƵĿ�����
//	scanAllValidMove:����������������װ,����ֵΪ�ɳ��ӵ�����,�����򷵻�0			
//-------------------------------------------------------------

void DFS_validEat(char curBoard1[BOARD_SIZE][BOARD_SIZE], int x, int y, int flag, int step, int turn)//�����Ѳ��ҿɳԵ���
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
		{//�ж���������:�˷������ӷ�?��һ���շ�?���ڷ�?
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
			if (curBoard1[x][y] == 2 + curFlag)//�������������һ������ʱ
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
			else if (curBoard1[x][y] == curFlag) {//�������������һ������ͨ��ʱ
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
//	gameOver:�жϵ�ǰ�Ծ��Ƿ��Ѿ����������ڼ���С������
//	evaluation:��������,����ǰ���ƴ��
//	alphaBeta:��֦����
//	miniMax:����Сֵ����
//-------------------------------------------------------------

int blackScore = 0, whiteScore = 0;

int gameOver(const char curBoard1[BOARD_SIZE][BOARD_SIZE], int curTurn)
//�ж���Ϸ�Ƿ��������������ֱ�������ʤ���Ƿ���δ���������0.
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

int evaluate(const char curBoard1[BOARD_SIZE][BOARD_SIZE], int curFlag, int turn)//��ֵ��������curBoard1���
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
void createRandomArray(int* array, int length)//����һ���������
{
	srand((unsigned)time(NULL));
	for (int i = 0; i < length; i++)
	{
		array[i] = i;//����һ����Ȼ����
	}
	for (int i = 0; i < length; i++)
	{
		int temp = rand() % (length - i) + i;
		swap(&array[temp], &array[i]);
	}
}
int alphaBeta(char curBoard1[BOARD_SIZE][BOARD_SIZE], int turn, int depth, int flag, int alpha, int beta)//alphabeta��֦
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
	//����ab��֦
	if (flag == me_flag)
	{
		if (validEatStep[turn])//�гԱس�
		{
			giveNumberMaxStep(turn);
			for (int i = 0; i <= validEat[turn][0].allPossibility; i++)
			{
				if (validEat[turn][i].numStep == validEat[turn][i].maxStep)
				{
					int tempNumRemaining[5] = { 0 };
					giveNumRemaining(virtualNumRemaining, tempNumRemaining, VIRTUALARRAYLENGTH);
					char tmpBoard[8][8] = { 0 };//��ʱ���̣����ڻָ�����֮ǰ�����̣��������ʱ����ͬ��
					giveBoard((char(*)[BOARD_SIZE])curBoard1, (char(*)[BOARD_SIZE])tmpBoard);
					placeCurBoard((char(*)[BOARD_SIZE])curBoard1, validEat[turn][i], flag);
					int value = alphaBeta((char(*)[BOARD_SIZE])curBoard1, turn + 1, depth - 1, other_flag, alpha, beta);
					giveBoard((char(*)[BOARD_SIZE])tmpBoard, (char(*)[BOARD_SIZE])curBoard1);//�ָ�֮ǰ�����̡�
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
		if (validEatStep[turn])//�гԱس�
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

int miniMax(const char curBoard1[BOARD_SIZE][BOARD_SIZE], int depth)//����Сֵ����
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
	if (thisNodeValue < 0)//���ݵ�ǰ���������������������
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
	giveBoard((char(*)[BOARD_SIZE])board, (char(*)[BOARD_SIZE])curBoard1);//����ʵ�����̱�Ϊ��������,�����������ϲ�����
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