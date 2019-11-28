#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
struct Command
	//define a command struct
	//using it to input & output the command of chess.
{
	int x[MAX_STEP];
	int y[MAX_STEP];
	int numStep;
};
char curBoard[BOARD_SIZE][BOARD_SIZE] = { 0 };//用来操作的临时棋盘
int curTurn;//记录现在的回合数
char board[BOARD_SIZE][BOARD_SIZE] = { 0 };
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
int direction[4][2] = { {-1,-1},{-1,1},{1,-1},{1,1} };
BOOL is_empty(int x, int y)
{
	if (!(x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE))
		return TRUE;
	if (curBoard[x][y])
		return FALSE;
	return TRUE;
}
BOOL is_mine(int x, int y, int me)
{
	int my_king;
	my_king = me + 2;
	if (board[x][y] == me || board[x][y] == my_king)
		return TRUE;
	return FALSE;
}
BOOL is_others(int x, int y, int others)
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
//evaluation:评估函数,给当前局势打分
//-------------------------------------------------------------
//各种常量
#define KINGPOINT 50
#define POWNPOINT 20
#define INFINITY 99999
int gameOver(char curBoard[BOARD_SIZE][BOARD_SIZE],int curTurn)
{
	int blackScore, whiteScore;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			switch (curBoard[i][j])
			{
			case 1:

			}
		}
	}
	if (curTurn >= 60)
	{
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			for (int j = 0; j < BOARD_SIZE; j++)
			{
				switch (curBoard[i][j])
				{
				case :

				}
			}
		}
	}

}
void evaluate()
{
	
}
//评估函数,评估当前局势(估值)
//-------------------------------------------------------------
//
//  DFS_ValidEat:查找多重吃子的可能性
//	scanValidEat:通过初始化查找多重吃子
//  scanValidNormalMove:查找普通位移的可能性
//-------------------------------------------------------------
char nextX, nextY;
struct Command ValidEat[MAX_TURN][MAX_STEP];
int possibility;
void DFS_ValidEat(int x, int y, int flag, int step, int turn)
//用深搜查找可吃的子
{
	int otherFlag = 3 - flag;
	ValidEat[turn][possibility].x[step - 1] = x;
	ValidEat[turn][possibility].y[step - 1] = y;
	ValidEat[turn][possibility].numStep = step;
	for (int k = 0; k < 4; k++)
	{
		nextX = x + direction[k][0];
		nextY = y + direction[k][1];
		if (is_others(nextX, nextY, otherFlag) &&
			is_empty(nextX + direction[k][0], nextY + direction[k][1]) &&
			isInBound(nextX + direction[k][0], nextY + direction[k][1]))
		{//判断三个条件:此方向有子否?空否?界内否?
			int temp = 0;
			temp = curBoard[nextX][nextY];
			curBoard[nextX][nextY] = 0;
			DFS_ValidEat(nextX + direction[k][0], nextY + direction[k][1], flag, step + 1, turn);
			possibility++;
			for (int i = 0; i < ValidEat[turn][possibility - 1].numStep; i++)
			{
				ValidEat[turn][possibility].x[i] = ValidEat[turn][possibility - 1].x[i];
				ValidEat[turn][possibility].y[i] = ValidEat[turn][possibility - 1].y[i];
			}
			curBoard[nextX][nextY] = temp;
		}
	}
}
void scanValidEat(int flag, int turn)
{
	possibility = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (curBoard[i][j] == flag)
			{
				DFS_ValidEat(i, j, flag, 1, turn);
			}
		}
	}
}
struct Command validNormalMove[MAX_TURN][MAX_STEP];
void scanValidNormalMove(int curFlag, int turn)
{
	possibility = 0;
	for (int x = 0; x < BOARD_SIZE; x++) {
		for (int y = 0; y < BOARD_SIZE; y++) {
			if (board[x][y] == 2 + curFlag)
			{//当这个棋子是这一方的王时
				for (int i = 0; i < 4; i++)
				{
					nextX = x + direction[i][0];
					nextY = y + direction[i][1];
					if (is_empty(nextX, nextY) && isInBound(nextX, nextY))
					{
						validNormalMove[turn][possibility].x[0] = x;
						validNormalMove[turn][possibility].x[1] = nextX;
						validNormalMove[turn][possibility].y[0] = y;
						validNormalMove[turn][possibility].y[1] = nextY;
						validNormalMove[turn][possibility].numStep = 2;
						possibility++;
					}
				}
			}
			else if (board[x][y] == curFlag) {//当这个棋子是这一方的普通子时
				for (int i = 2 * curFlag - 2; i < 2 * curFlag; i++)
				{
					nextX = x + direction[i][0];
					nextY = y + direction[i][1];
					if (is_empty(nextX, nextY) && isInBound(nextX, nextY))
					{
						validNormalMove[turn][possibility].x[0] = x;
						validNormalMove[turn][possibility].x[1] = nextX;
						validNormalMove[turn][possibility].y[0] = y;
						validNormalMove[turn][possibility].y[1] = nextY;
						validNormalMove[turn][possibility].numStep = 2;
						possibility++;
					}
				}
			}
		}
	}
}
struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me)
{
	scanValidEat(me, curTurn);
	startTime = clock();
	int maxStep = 0;
	int maxIndex = 0;
	for (int i = 0; i <= possibility; i++)
	{
		if (ValidEat[curTurn][i].numStep > maxStep) 
		{
			maxStep = ValidEat[curTurn][i].numStep;
			maxIndex = i;
		}
	}
	if (maxStep >= 2)
	{
		return ValidEat[curTurn][maxIndex];
	}
	else 
	{
		scanValidNormalMove(me, curTurn);
		return validNormalMove[curTurn][0];
	}
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
	printf("\n%lf\n", (double)(startTime - endTime));
	fflush(stdout);
}

void end(int x)
{

}

void loop()
{
	//  freopen("../input", "r", stdin);
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