#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <string.h>

#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'
#define PAUSE ' '
#define WIDTH 52
#define HEIGHT 27
#define NAME_SIZE 128
#define MAX_PLAYER 1024

// * 链表实现贪吃蛇身体
typedef struct Snake
{
	int x;
	int y;
	struct Snake *next;
}Snake;
// * 将食物的坐标和分数存在一个结构体里面，食物的分数高低由难度决定
typedef struct food
{
	int x;
	int y;
	int score;
}Food;

typedef struct playerInfo
{
	char name[NAME_SIZE];
	int high_score;
	int play_times;
}Player;



// ************************************************************************
// * 
// * 游戏中的全局变量
// *
// ************************************************************************
char cGameScene[HEIGHT][WIDTH];             // 设计游戏场景大小为 50 x 25 
char cUserName[NAME_SIZE];
int iTotalScore;
int iLastScore;
int iTotalTimes = 0;

int iDifficulty = 1;
int iRefreshTimes[4] = { 50, 25, 5, 0 };
int iGameOver = 0;
int iFoodSpawned = 0;
char cControl;
Snake *head, *tail;
Food food;

// ************************************************************************
// * 
// * 函数声明
// *
// ************************************************************************
void initGame();             // 初始化游戏场景，和全局变量
void refreshScreen();        // 每一帧的屏幕刷新
void gameLoop();             // 游戏逻辑主循环

void enterUsername();                            // 刚进入游戏时，选择是新建玩家还是读取存档
void startMenu();                                // 开始游戏后的选择菜单
void chooseDifficulty();                         // 选择游戏难度
void displayScoreBoard();                        // 显示排行榜
void myInfo();                                   // 显示我的信息
void gameOver();                                 // 游戏结束页面
void updateScoreBoard();                         // 更新排行榜

void updateSnakeShape();
int randint(int min, int max);
int ifGameOver();
void spawnFood();

void gotoxy(int x, int y);

int main(void)
{
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
	srand((unsigned int)time(0));
	enterUsername();
	while (1)
	{
		startMenu();
		
		system("cls");
		initGame();

		gameLoop();
		gameOver();
		system("cls");
	}
	return 0;
}

void gotoxy(int x, int y) {
	COORD pos = { x,y };
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);// 获取标准输出设备句柄
	SetConsoleCursorPosition(hOut, pos);//两个参数分别是指定哪个窗体，具体位置
}

int randint(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

void enterUsername()
{
	int iSelect;
	char cSearchName[NAME_SIZE], cName[NAME_SIZE];
	FILE *fp;
	int iScore;
	int iFound = 0;

	system("cls");
	printf("====================================================\n");
	printf("#                                                  #\n");
	printf("#                                                  #\n");
	printf("#                  1. 建立新玩家                   #\n");
	printf("#                  2. 读取存档                     #\n");
	printf("#                                                  #\n");
	printf("#                                                  #\n");
	printf("====================================================\n");
	printf("                  选项：");

	scanf("%d", &iSelect);
	getchar();
	fflush(stdin);
	if (iSelect == 1)
	{
		system("cls");
		printf("\t\t输入用户名：");
		gets(cUserName);
	}
	else if (iSelect == 2)
	{
		system("cls");
		if ((fp = fopen("scoreBoard.txt", "r+")) == NULL)
		{
			printf("\t\t存档文件读取失败，请检查游戏完整性.\n");
			printf("\t\t按任意键退出游戏\n");
			char ch = _getch();
			exit(1);
		}
		
		printf("\t\t输入存档名：");
		gets(cSearchName);

		while (!feof(fp))
		{
			fscanf(fp, "%s%d%d\n", cName, &iScore, &iTotalTimes);
			if (strcmp(cSearchName, cName) == 0)
			{
				iFound = 1;
				strcpy(cUserName, cSearchName);
				iLastScore = iScore;
				printf("\t\t找到玩家：%s, 当前分数：%d, 游戏总次数：%d\n", cUserName, iLastScore, iTotalTimes);
				printf("\t\t按任意键继续.\n");
				char ch = _getch();
				break;
			}
		}
		if (iFound == 0)
		{
			printf("\t\t没有找到该存档信息，请检查是否正确输入存档名.\n");
			printf("                    按任意键返回初始菜单\n");
			char ch = _getch();
			enterUsername();
		}
		fclose(fp);
	}
	else
	{
		printf("                    选项错误\n");
		printf("                    按任意键返回初始菜单\n");
		char ch = _getch();
		enterUsername();
	}
}

void startMenu()
{
	int iSelect;

	system("cls");
	printf("====================================================\n");
	printf("#                                                  #\n");
	printf("#                  贪吃蛇 Version Alpha            #\n");
	printf("#                                                  #\n");
	printf("#                1. 开始游戏                       #\n");
	printf("#                2. 查看排行榜                     #\n");
	printf("#                3. 查看我的分数                   #\n");
	printf("#                4. 切换玩家                       #\n");
	printf("#                5. 退出游戏                       #\n");
	printf("#                                                  #\n");
	printf("#                                                  #\n");
	printf("====================================================\n");

	printf("                  选项：");
	scanf("%d", &iSelect);

	system("cls");
	if (iSelect == 1)
	{
		chooseDifficulty();
		iTotalTimes += 1;
		printf("                  Good Luck ! ! !\n");
		Sleep(2000);
	}
	else if (iSelect == 2)
	{
		displayScoreBoard();
		printf("                    按任意键返回开始界面\n");
		char ch = _getch();
		startMenu();
	}
	else if (iSelect == 3)
	{
		myInfo();
		printf("                    按任意键返回开始界面\n");
		char ch = _getch();
		startMenu();
	}
	else if (iSelect == 5)
	{
		printf("\t\t正在将你的分数写入排行榜，请勿关闭游戏\n");
		updateScoreBoard();
		Sleep(500);
		printf("                    按任意键退出游戏\n");
		char ch = _getch();
		exit(1);
	}
	else if (iSelect == 4)
	{
		updateScoreBoard();
		enterUsername();
		startMenu();
	}
	else
	{
		printf("                    选项错误\n");
		printf("                    按任意键返回开始界面\n");
		fflush(stdin);
		char ch = _getch();
		startMenu();
	}
}

void chooseDifficulty()
{
	int iSelect;

	printf("           1. 简单  2. 普通  3. 困难  4. 噩梦\n");
	printf("                  选项：");
	scanf("%d", &iSelect);

	if (iSelect <= 4 && iSelect >= 1)
	{
		iDifficulty = iSelect;
	}
	else
	{
		printf("             请正确输入选项\n");
		printf("                    按任意键重选\n");
		char ch = _getch();
		chooseDifficulty();
	}
}

void displayScoreBoard()
{
	FILE *fp;
	char cName[NAME_SIZE];
	int iScore;
	int iTimes;

	if ((fp = fopen("scoreBoard.txt", "r+")) == NULL)
	{
		printf("\t\t存档文件读取失败，请检查游戏完整性.\n");
		printf("\t\t按任意键退出游戏\n");
		char ch = _getch();
		exit(1);
	}
	
	system("cls");
	printf("======================  排行榜  ====================\n");
	while (!feof(fp))
	{
		fscanf(fp, "%s%d%d\n", cName, &iScore, &iTimes);
		printf("               %s : %d\n", cName, iScore);
	}
	printf("====================================================\n");
	fclose(fp);
}

void updateScoreBoard()
{
	FILE *fp;
	Player playerList[MAX_PLAYER];
	int iPlayerCount = 0;
	int iFound = 0;
	int i, j;


	if ((fp = fopen("scoreBoard.txt", "r+")) == NULL)
	{
		printf("\t\t存档文件读取失败，请检查游戏完整性.\n");
		printf("\t\t按任意键退出游戏\n");
		char ch = _getch();
		exit(1);
	}

	while (!feof(fp))
	{
		fscanf(fp, "%s%d%d\n", playerList[iPlayerCount].name, &playerList[iPlayerCount].high_score, &playerList[iPlayerCount].play_times);
		if (strcmp(cUserName, playerList[iPlayerCount].name) == 0)
		{
			iFound = 1;
			if (iTotalScore > playerList[iPlayerCount].high_score)
			{
				playerList[iPlayerCount].high_score = iTotalScore;
			}
			playerList[iPlayerCount].play_times = iTotalTimes;
		}
		
		iPlayerCount++;
	}
	if (iFound == 0)
	{
		playerList[iPlayerCount].high_score = iTotalScore;
		strcpy(playerList[iPlayerCount].name, cUserName);
		playerList[iPlayerCount].play_times = iTotalTimes;
		iPlayerCount++;
	}

	for (i = 0; i < iPlayerCount; i++)
	{
		for (j = 0; j < iPlayerCount - i - 1; j++)
		{
			if (playerList[j].high_score < playerList[j + 1].high_score)
			{
				Player temp = playerList[j];
				playerList[j] = playerList[j + 1];
				playerList[j + 1] = temp;
			}
		}
	}
	i = 0;
	fseek(fp, 0, SEEK_SET);
	while (i < iPlayerCount)
	{
		fprintf(fp, "%s %d %d\n", playerList[i].name, playerList[i].high_score, playerList[i].play_times);
		i++;
	}
	fclose(fp);
}

void myInfo()
{
	int iScore;
	char cName[NAME_SIZE];	
	FILE *fp;
	int iFound = 0;

	if ((fp = fopen("scoreBoard.txt", "r+")) == NULL)
	{
		printf("\t\t存档文件读取失败，请检查游戏完整性.\n");
		printf("\t\t按任意键退出游戏\n");
		char ch = _getch();
		exit(1);
	}

	while (!feof(fp))
	{
		fscanf(fp, "%s%d\n", cName, &iScore);
		if (strcmp(cName, cUserName) == 0)
		{
			iFound = 1;
			iLastScore = iScore;
			printf("\t\t玩家：%s,  历史最高分：%d,  游戏次数: %d\n", cUserName, iLastScore, iTotalTimes);
			break;
		}
	}
	if (iFound == 0)
	{
		if (iFound == 0)
		{
			printf("\t\t玩家：%s,  当前总分：%d,  游戏次数：%d\n", cUserName, iTotalScore, iTotalTimes);
			printf("\t\t你的信息暂时未写入存档，请稍候... ...\n");
			updateScoreBoard();
			Sleep(1000);
		}
	}
	fclose(fp);
}

void gameOver()
{
	system("cls");
	printf("====================================================\n\n\n\n");
	printf("                      GAME OVER                     \n\n");
	printf("                  总分：%4d\n\n\n", iTotalScore);
	printf("====================================================\n");
	
	if (iTotalScore > iLastScore)
	{
		updateScoreBoard();
		iLastScore = iTotalScore;
	}
	printf("                    按任意键返回开始界面\n");
	char ch = _getch();
	iFoodSpawned = 0;
	iTotalScore = 0;
}

void initGame()
{
	int i, j;

	for (i = 0; i < HEIGHT; i++)
	{
		for (j = 0; j < WIDTH; j++)
		{
			if (i == 0 || j == 0 || i == HEIGHT - 1 || j == WIDTH - 1)
			{
				cGameScene[i][j] = '#';
			}
			else
			{
				cGameScene[i][j] = ' ';
			}
		}
	}
	head = (Snake*)malloc(sizeof(Snake));
	head->x = 24;
	head->y = 12;
	head->next = (Snake*)malloc(sizeof(Snake));
	head->next->x = 23;
	head->next->y = 12;
	head->next->next = NULL;
	tail = head->next;
	for (i = 0; i < HEIGHT; i++)
	{
		for (j = 0; j < WIDTH; j++)
		{
			putchar(cGameScene[i][j]);
		}
		putchar('\n');
	}
}

void refreshScreen()
{
	int i, j;

	for (i = 1; i < HEIGHT - 1; i++)
	{
		for (j = 1; j < WIDTH - 1; j++)
		{
			gotoxy(j, i);
			putchar(cGameScene[i][j]);
		}
	}
	gotoxy(WIDTH + 5, 12);
	printf("得分：%d", iTotalScore);
}

void updateSnakeShape()
{
	Snake *node = head;
	int x, y, preX = head->x, preY = head->y;
	int i, j;

	for (i = 1; i < HEIGHT - 1; i++)
	{
		for (j = 1; j < WIDTH - 1; j++)
		{
			if (cGameScene[i][j] != '*')
			{
				cGameScene[i][j] = ' ';
			}
		}
	}
	if (head->next != NULL && (cControl == UP || cControl == DOWN || cControl == LEFT || cControl == RIGHT))
	{
		while (node != NULL)
		{
			x = node->x;
			y = node->y;
			node->x = preX;
			node->y = preY;
			preX = x;
			preY = y;
			node = node->next;
		}
	}
	node = head->next;
	while (node != NULL)
	{
		cGameScene[node->y][node->x] = 'O';
		node = node->next;
	}
}

int ifGameOver()
{
	Snake *node = head->next;

	if (head->x == WIDTH - 1 || head->x == 0 || head->y == HEIGHT - 1 || head->y == 0)
	{
		return 1;
	}
	while (node != NULL)
	{
		if (head->x == node->x && head->y == node->y)
		{
			return 1;
		}
		node = node->next;
	}
	return 0;
}

void spawnFood()
{
	food.x = randint(1, WIDTH - 2);
	food.y = randint(1, HEIGHT - 2);
	food.score = iDifficulty;

	Snake *node = head;
	while (node != NULL)
	{
		if (node->x == food.x && node->y == food.y)
		{
			food.x = randint(1, WIDTH - 2);
			food.y = randint(1, HEIGHT - 2);
			node = head;
			continue;
		}
		node = node->next;
	}

	cGameScene[food.y][food.x] = '*';

	iFoodSpawned = 1;
}

void gameLoop()
{
	
	while (!ifGameOver())
	{
		Sleep(iRefreshTimes[iDifficulty - 1]);
		if (_kbhit())
		{
			cControl = _getch();
		}
		updateSnakeShape();

		switch (cControl)
		{
		case UP: head->y -= 1; break;
		case DOWN: head->y += 1; break;
		case LEFT: head->x -= 1; break;
		case RIGHT: head->x += 1; break;
		default: break;
		}
		cGameScene[head->y][head->x] = '@';

		if (iFoodSpawned == 0)
		{
			spawnFood();
		}

		if (head->x == food.x && head->y == food.y)
		{
			iTotalScore += 4;
			tail->next = (Snake*)malloc(sizeof(Snake));
			switch (cControl)
			{
			case UP: tail->next->y = tail->y + 1; tail->next->x = tail->x; break;
			case DOWN: tail->next->y = tail->y - 1; tail->next->x = tail->x; break;
			case LEFT: tail->next->y = tail->y; tail->next->x = tail->x + 1; break;
			case RIGHT: tail->next->y = tail->y; tail->next->x = tail->x - 1; break;
			}
			tail = tail->next;
			tail->next = NULL;
			iFoodSpawned = 0;
		}
		refreshScreen();
	}
}