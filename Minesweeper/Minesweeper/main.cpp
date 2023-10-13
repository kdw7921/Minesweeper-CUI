#pragma warning(disable:4996)

#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<time.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>

#define mapX 2
#define mapY 2

#define UP 72
#define DOWN 80
#define RIGHT 77
#define LEFT 75
#define Pause 80
#define pause 112
#define SPACE 32
#define ENTER 13
#define BACKSPACE 8
#define ESC 27
#define Key_Z 122
#define Key_S 115

int LINE = 40;
int COLS = 125;
char **Map = NULL;
char **isClicked = NULL;
int level, N, Mn,count;

time_t playTime, startTime;

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE;

void gotoxy(int, int);				// Set the CursorPosition
void setcursortype(CURSOR_TYPE c);	// Set the condition of Cursor

void drawMinesweeper(int, int);
void Intro();
int mainDraw(int);					// Draw the Title 
void records();
void option();
void credits();
int Playgame();
int EndGame();
int typeCoord(int *x, int *y);
void zeroBlock(int x, int y);
void drawMap(int x, int y);

int main()
{
	char buf[256] = { 0 };
	sprintf(buf, "mode con: lines=%d cols=%d", LINE, COLS);
	system(buf);
	srand(time(NULL));
	setcursortype(NOCURSOR);		//Set the Cursor Invisible

	FILE* fp = fopen("records0.txt", "rt");
	if (!fp) {
		fp = fopen("records0.txt", "wt");
		for (int i = 0; i<40; i++)	fprintf(fp, "Empty 0\n");
	}
	fclose(fp);
	fp = fopen("records1.txt", "rt");
	if (!fp) {
		fp = fopen("records1.txt", "wt");
		for (int i = 0; i<40; i++)	fprintf(fp, "Empty 0\n");
	}
	fclose(fp);
	fp = fopen("records2.txt", "rt");
	if (!fp) {
		fp = fopen("records2.txt", "wt");
		for (int i = 0; i<40; i++)	fprintf(fp, "Empty 0\n");
	}
	fclose(fp);
	int res;

	Intro(); //인트로 

			 //프로그램 전체적인 흐름 
	while (1) {
		res = 0;
		//메인 화면 
		while (1) {
			res = mainDraw(res);
			if (res == 0) break; //싱글 플레이 선택
			//if (res == 6) break; //온라인 플레이 선택
			else if (res == 1) records();
			else if (res == 2) option();
			else if (res == 3) credits();
			else if (res == 4) {
				res = 5;
				break;
			}
		}
		if (res == 5) break;

		//게임 시작 
		while (1) {
			res = Playgame();
			if (res == 1) break;
			else if (res == 2) {
				res = 5;
				break;
			}
		}
		if (res == 5) break;
	}

	//프로그램 종료 
	gotoxy(0, LINE - 5);
	exit(0);
}
int Playgame() {
	int x = COLS / 8 - 12, y = LINE / 4 - 5;
	int i, j;
	level = 0;
	count = 0;
	system("cls");
	gotoxy(x + 17, y + 12); printf("┌-------------------┐");
	gotoxy(x + 17, y + 13); printf("│   easy   (16x16)  │");
	gotoxy(x + 17, y + 14); printf("│   normal (32x32)  │");
	gotoxy(x + 17, y + 15); printf("│   hard   (64x64)  │");
	gotoxy(x + 17, y + 16); printf("└-------------------┘");
	int in;
	while (1) {
		gotoxy(x + 18, y + 13 + level); printf(" >");
		in = getch();
		gotoxy(x + 18, y + 13 + level); printf("  ");
		if (in == 224) {
			in = getch();
			if (in == DOWN) level = (level + 1) % 3;
			if (in == UP) level = (level + 2) % 3;
		}
		if (in == ENTER) break;
		if (in == ESC) return 1;
	}

	//동적할당 초기화 
	if (isClicked != NULL) {
		free(isClicked[0]);
		free(isClicked);
		isClicked = NULL;
	}
	if (Map != NULL) {
		free(Map[0]);
		free(Map);
		Map = NULL;
	}

	//맵 초기화 
	int x_, y_;
	{
		//level 체크
		if (level == 0) {
			N = 16;
			Mn = 40;
		}
		else if (level == 1) {
			N = 22;
			Mn = 99;
		}
		else {
			N = 30;
			Mn = 240;
		}

		//맵 크기 할당 
		Map = (char**)malloc(sizeof(char*)*N);
		for (i = 0; i<N; i++) Map[i] = (char*)malloc(sizeof(char)*N);
		isClicked = (char**)malloc(sizeof(char*)*N);
		for (i = 0; i<N; i++) isClicked[i] = (char*)malloc(sizeof(char)*N);

		//배열 초기화 
		for (i = 0; i<N; i++)
			for (j = 0; j<N; j++) isClicked[i][j] = 0;
		for (i = 0; i<N; i++)
			for (j = 0; j<N; j++) Map[i][j] = 0;

		//처음 입력 받기
		system("cls");
		x_ = 0;
		y_ = 0;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 128);
		for (i = 0; i<N; i++) {
			gotoxy(COLS / 8 - 12, LINE / 4 - 5 + i);
			for (j = 0; j<N; j++) printf("▣");
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10 | 128);
		gotoxy(0, 0); printf(" time: 0          ");
		gotoxy(9, 0); printf(" LeftBlock: %3d ", N*N-Mn); //현재 '경과된 시간'과 '남은 블록 갯수' 출력
		gotoxy(0, 1); printf("                                  ");
		
		//함수 어쩔 수 없이 그대로 다시 가져옴
		{
			int ch;
			gotoxy((x_) + COLS / 8 - 12, y_ + LINE / 4 - 5);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14 | 112);
			printf("▣");
			while (1) {
				ch = getch();

				//원래 표시 지우기 
				gotoxy(COLS / 8 - 12 + (x_), y_ + LINE / 4 - 5);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 128);
				printf("▣");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7 | 0);

				//입력 받기 
				if (ch == 224) {
					ch = getch();
					switch (ch) {
					case RIGHT: {
						if (x_<N - 1) (x_)++;
						break;
					}
					case LEFT: {
						if (x_>0) (x_)--;
						break;
					}
					case DOWN: {
						if (y_<N - 1) (y_)++;
						break;
					}
					case UP: {
						if (y_>0) (y_)--;
						break;
					}
					}
				}
				else if (ch == Key_Z) break;
				else if (ch == ESC) return 1;
				//표시하기 
				gotoxy(COLS / 8 - 12 + (x_), LINE / 4 - 5 + y_);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14 | 112);
				printf("▣");
			}
		}
		startTime = clock();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

		//지뢰 심기
		{
			int mx, my;
			for (i = 0; i<Mn; i++) {
				mx = rand() % N;
				my = rand() % N;
				if (Map[my][mx] == 9 || (x_ - 1 <= mx && x_ + 1 >= mx && y_ - 1 <= my && y_ + 1 >= my)) i--;
				else Map[my][mx] = 9;
			}
		}
		//게임판 세팅	
		int num;
		for (i = 0; i<N; i++) {
			for (j = 0; j<N; j++) {
				if (Map[i][j] != 9) {
					num = 0;
					if (i - 1 >= 0 && j - 1 >= 0) //왼쪽 위 
						if (Map[i - 1][j - 1] == 9) num++;
					if (i - 1 >= 0) //위 
						if (Map[i - 1][j] == 9) num++;
					if (i - 1 >= 0 && j + 1<N) //오른쪽 위 
						if (Map[i - 1][j + 1] == 9) num++;
					if (j - 1 >= 0) //왼쪽 
						if (Map[i][j - 1] == 9) num++;
					if (j + 1<N) //오른쪽 
						if (Map[i][j + 1] == 9) num++;
					if (i + 1<N&&j - 1 >= 0) //왼쪽 아래 
						if (Map[i + 1][j - 1] == 9) num++;
					if (i + 1<N) //아래 
						if (Map[i + 1][j] == 9) num++;
					if (i + 1<N&&j + 1<N) //오른쪽 아래 
						if (Map[i + 1][j + 1] == 9) num++;
					Map[i][j] = num;
				}
			}
		}

		isClicked[y_][x_] = 1;
		zeroBlock(x_, y_);
	}

	//맵그리기
	system("cls");
	drawMap(x, y);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7 | 0);

	count = 0;
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			if (isClicked[i][j] == 1) count++;

	//게임 루틴
	char name[40][20];
	int	Records[40];
	for (i = 0; i < 40; i++) {
		strcpy(name[i], "Empty");
		Records[i] = 0;
	}
	char filename[] = "records .txt";
	filename[7] = (char)(48 + level);
	while (1) {
		in = typeCoord(&x_, &y_);
		if (in == ESC) return 1;
		if (in == Key_S) {
			if (isClicked[y_][x_] == 0) isClicked[y_][x_] = 2;
			else if (isClicked[y_][x_] == 2) isClicked[y_][x_] = 0;
			gotoxy(x + x_, y + y_); 
			printf("▶");
		}
		else if (in == Key_Z) {
			if (isClicked[y_][x_] == 0) {
				if (Map[y_][x_] == 9) {
					for (i = 0; i<N; i++) {
						gotoxy(x, y + i);
						for (j = 0; j<N; j++) {
							if (isClicked[i][j] == 1)
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Map[i][j] | 112);
							else
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Map[i][j] | 128);
							if (Map[i][j] == 9) printf("＠");
							else if (Map[i][j] == 0) printf("  ");
							else printf("%02d", Map[i][j]);
						}
					}
					gotoxy(x + x_, y + y_);
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4 | 192);
					printf("＠");
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7 | 0);
					break;
				}
				else if (Map[y_][x_] == 0) zeroBlock(x_, y_);	//0 블록 선택 
				isClicked[y_][x_] = 1;	//flag 배열 변환 
				drawMap(x, y);	//맵그리기  

				count = 0;
				int i, j;
				for (i = 0; i < N; i++)
					for (j = 0; j < N; j++)
						if (isClicked[i][j] == 1) count++;

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7 | 0);

				//게임 클리어 
				if (count == N * N - Mn) {
					playTime = (clock() - startTime)/CLOCKS_PER_SEC;
					FILE* buf = fopen(filename, "rt");
					for (i = 0; i<40; i++)	fscanf(buf, "%s %d", name[i], &Records[i]);
					fclose(buf);
					buf = fopen(filename, "wt");
					for (i = 0; i<40; i++) {
						if (playTime<Records[i]||Records[i]<=0) {
							for (j = 39; j>i; j--) {
								strcpy(name[j], name[j - 1]);
								Records[j] = Records[j - 1];
							}
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7 | 128);
							gotoxy(mapX + 30, mapY + 7); printf(" type your name :                     "); 
							gotoxy(mapX + 30, mapY + 8); printf(" playtime: %d (sec)   ", playTime);
							gotoxy(mapX + 41, mapY + 8); printf("                ");
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10 | 128);
							while (kbhit()) getch();
							gotoxy(mapX + 39, mapY + 7);	scanf("%s", name[i]);
							while (kbhit()) getch();
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7 | 0);
							gotoxy(mapX + 30, mapY + 7); printf("                                      ");
							gotoxy(mapX + 30, mapY + 8); printf("                                      ");
							while (kbhit()) getch();
							Records[i] = playTime;
							for (i = 0; i<40; i++)	fprintf(buf, "%s  %d\n", name[i], Records[i]);
							i = 40;
						}
					}
					fclose(buf);
					break;
				}
			}
		}
	}
	for (j = 0; j<20; j++) {
		gotoxy(mapX + 40, j + 3);
		printf("%02d) %-20s %d", j + 1, name[j], Records[j]);
	}
	if (playTime == Records[i]) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10 | 0);
		gotoxy(mapX + 40, i + 3); printf("%02d) %-20s %d", i + 1, name[i], Records[i]);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7 | 0);
	}
	return EndGame();
}

void drawMinesweeper(int x, int y) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11 | 0);
	gotoxy(x + 5, y);     printf(" ■ ■■■ ■■    ■■   ■           ■■■■■");
	gotoxy(x + 5, y + 1); printf("  ■■   ■   ■           ■■■■    ■■      ");
	gotoxy(x + 5, y + 2); printf(" ■■    ■   ■   ■■   ■      ■   ■■■■■");
	gotoxy(x + 5, y + 3); printf(" ■■    ■   ■   ■■   ■      ■   ■■      ");
	gotoxy(x + 5, y + 4); printf(" ■■    ■   ■   ■■   ■      ■   ■■■■■");

	gotoxy(x, y + 6); printf("  ■■■   ■     ■■    ■   ■■■■   ■■■■   ■■■   ■■■■   ■■■  ");
	gotoxy(x, y + 7); printf(" ■         ■   ■ ■   ■    ■         ■         ■   ■  ■         ■   ■ ");
	gotoxy(x, y + 8); printf("  ■■■     ■  ■  ■ ■     ■■■■   ■■■■   ■■■   ■■■■   ■■■  ");
	gotoxy(x, y + 9); printf("       ■    ■ ■   ■ ■     ■         ■         ■       ■         ■   ■ ");
	gotoxy(x, y + 10); printf("■■■■      ■      ■       ■■■■   ■■■■   ■       ■■■■   ■    ■");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7 | 0);
	return;
}
void Intro() {
	system("cls");
	int x = COLS / 8 - 3, y = LINE / 4;
	drawMinesweeper(x, y);
	gotoxy(x + 12, y + 16);	printf("<press any key to start>");
	int count = 0;
	while (1) {
		Sleep(10);
		count++;
		gotoxy(x + 12, y + 16);
		if ((count % 100) == 0)    printf("<press any key to start>");
		else if ((count % 100 - 50) == 0) printf("                        ");

		if (kbhit()) {
			getch();
			gotoxy(x + 12, y + 16);	printf("                        ");
			return;
		}
	}
}
int mainDraw(int ch) {
	system("cls");
	int x = COLS / 8 - 3, y = LINE / 4;
	int starty;
	drawMinesweeper(x, y);
	gotoxy(x + 15, y + 13); printf("GameStart");
	gotoxy(x + 15, y + 14); printf("Records");
	gotoxy(x + 15, y + 15); printf("Option");
	gotoxy(x + 15, y + 16); printf("Credits");
	gotoxy(x + 15, y + 17); printf("Exit");
	int p;
	while (1) {
		gotoxy(x + 14, ch + y + 13);	printf(" >");
		p = getch();
		gotoxy(x + 14, ch + y + 13);	printf("  ");
		if (p == 224) {
			p = getch();
			if (p == UP) ch = (ch + 4) % 5;
			if (p == DOWN) ch = (ch + 1) % 5;
		}
		else if (p == ENTER) {
			gotoxy(x + 14, ch + y + 13);
			printf(" >");
			if (ch == 0) {
				starty = 0;
				gotoxy(x + 20, y + 12); printf("┌--------------------┐");
				gotoxy(x + 20, y + 13); printf("│   Single Play      │");
				gotoxy(x + 20, y + 14); printf("│   Online MultiPlay │");
				gotoxy(x + 20, y + 15); printf("└--------------------┘");
				while (1) {
					gotoxy(x + 21, y + starty + 13); printf(" >");
					ch = getch();
					gotoxy(x + 21, y + starty + 13); printf("  ");
					if (ch == 224) {
						ch = getch();
						//if (ch == DOWN || ch == UP) starty = (starty + 1) % 2;
					}
					else if (ch == ENTER) return starty * 6;
					else if (ch == BACKSPACE || ch == ESC) {
						for (ch = 0; ch<4; ch++) {
							gotoxy(x + 20, y + 12 + ch); printf("                         ");
						}
						ch = 0;
						starty = 0;
						break;
					}
				}
			}
			else	return ch;
		}
	}
}
void drawMap(int x, int y) {
	int i, j;
	for (i = 0; i<N; i++) {
		gotoxy(x, y + i);
		for (j = 0; j<N; j++) {
			if (isClicked[i][j] == 1) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Map[i][j] | 112);
				if (Map[i][j] == 9) printf("＠");
				else if (Map[i][j] == 0)	printf("  ");
				else printf("%02d", Map[i][j]);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 128);
			}
			else if(isClicked[i][j] == 2){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12 | 112);
				printf("▶");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 128);
			}
			else {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 128);
				printf("▣");
			}
		}
	}
}
int typeCoord(int *x, int *y) {
	int ch;
	gotoxy((*x) + COLS / 8 - 12, *y + LINE / 4 - 5);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14 | 112);
	if (isClicked[*y][*x] == 1) {
		if (Map[*y][*x] == 0) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 224);
			printf("  ");
		}
		else printf("%02d", Map[*y][*x]);
	}
	else if(isClicked[*y][*x] == 2)	printf("▶");
	else	printf("▣");
	while (1) {
		if (kbhit()) {
			ch = getch();

			//원래 표시 지우기 
			gotoxy(COLS / 8 - 12 + (*x), *y + LINE / 4 - 5);
			if (isClicked[*y][*x] == 1) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Map[*y][*x] | 112);
				if (Map[*y][*x] == 0)	printf("  ");
				else printf("%02d", Map[*y][*x]);
			}
			else if (isClicked[*y][*x] == 2) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12 | 112);
				printf("▶");
			}
			else {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 128);
				printf("▣");
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7 | 0);
			//입력 받기 
			if (ch == 224) {
				ch = getch();
				switch (ch) {
				case RIGHT: {
					if (*x<N - 1) (*x)++;
					break;
				}
				case LEFT: {
					if (*x>0) (*x)--;
					break;
				}
				case DOWN: {
					if (*y<N - 1) (*y)++;
					break;
				}
				case UP: {
					if (*y>0) (*y)--;
					break;
				}
				}
			}
			else if (ch == Key_Z) return Key_Z;
			else if (ch == Key_S) return Key_S;
			else if (ch == ESC) return ESC;
			//표시하기 
			gotoxy(COLS / 8 - 12 + (*x), LINE / 4 - 5 + *y);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14 | 112);
			if (isClicked[*y][*x] == 1) {
				if (Map[*y][*x] == 0) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 224);
					printf("  ");
				}
				else printf("%02d", Map[*y][*x]);
			}
			else if (isClicked[*y][*x] == 2) printf("▶");
			else printf("▣");
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10 | 128);
		gotoxy(0, 0); printf(" time: %d          ", (clock() - startTime) / CLOCKS_PER_SEC);
		gotoxy(9, 0); printf(" LeftBlock: %3d ", N*N - Mn - count); //현재 '경과된 시간'과 '남은 블록 갯수' 출력
		gotoxy(0, 1); printf("                                  ");
	}
}
void zeroBlock(int x, int y) {
	if (y - 1 >= 0 && x - 1 >= 0) { //왼쪽 위 
		if (isClicked[y - 1][x - 1] == 0) {
			isClicked[y - 1][x - 1] = 1;
			if (Map[y - 1][x - 1] == 0) zeroBlock(x - 1, y - 1);
		}
	}
	if (y - 1 >= 0) //위 
		if (isClicked[y - 1][x] == 0) {
			isClicked[y - 1][x] = 1;
			if (Map[y - 1][x] == 0) zeroBlock(x, y - 1);
		}
	if (y - 1 >= 0 && x + 1<N) { //오른쪽 위 
		if (isClicked[y - 1][x + 1] == 0) {
			isClicked[y - 1][x + 1] = 1;
			if (Map[y - 1][x + 1] == 0) zeroBlock(x + 1, y - 1);
		}
	}
	if (x - 1 >= 0) { //왼쪽 
		if (isClicked[y][x - 1] == 0) {
			isClicked[y][x - 1] = 1;
			if (Map[y][x - 1] == 0) zeroBlock(x - 1, y);
		}
	}
	if (x + 1<N) { //오른쪽 
		if (isClicked[y][x + 1] == 0) {
			isClicked[y][x + 1] = 1;
			if (Map[y][x + 1] == 0) zeroBlock(x + 1, y);
		}
	}
	if (y + 1<N&&x - 1 >= 0) { //왼쪽 아래 
		if (isClicked[y + 1][x - 1] == 0) {
			isClicked[y + 1][x - 1] = 1;
			if (Map[y + 1][x - 1] == 0) zeroBlock(x - 1, y + 1);
		}
	}
	if (y + 1<N) { //아래 
		if (isClicked[y + 1][x] == 0) {
			isClicked[y + 1][x] = 1;
			if (Map[y + 1][x] == 0) zeroBlock(x, y + 1);
		}
	}
	if (x + 1<N&&y + 1<N) { //오른쪽 아래 
		if (isClicked[y + 1][x + 1] == 0) {
			isClicked[y + 1][x + 1] = 1;
			if (Map[y + 1][x + 1] == 0) zeroBlock(x + 1, y + 1);
		}
	}
}
void records() {
	system("cls");
	int i;
	int x = COLS / 8, y = LINE / 4;
	char name[20];
	memset(name, '\0', 20);
	int score;

	gotoxy(x + 10, y + 7);  printf("┌-------------------┐");
	gotoxy(x + 10, y + 8);  printf("│   easy   (16x16)  │");
	gotoxy(x + 10, y + 9);  printf("│   normal (32x32)  │");
	gotoxy(x + 10, y + 10); printf("│   hard   (64x64)  │");
	gotoxy(x + 10, y + 11); printf("└-------------------┘");
	int in;
	while (1) {
		gotoxy(x + 11, y + 8 + level); printf(" >");
		in = getch();
		gotoxy(x + 11, y + 8 + level); printf("  ");
		if (in == 224) {
			in = getch();
			if (in == DOWN) level = (level + 1) % 3;
			if (in == UP) level = (level + 2) % 3;
		}
		if (in == ENTER) break;
		if (in == ESC) return;
	}
	system("cls");
	char filename[] = "records .txt";
	filename[7]=(char)(48 + level);
	FILE* Rbuf = fopen(filename, "rt");
	gotoxy(1, 1); printf("%s",level==0?"easy":(level==1?"normal":"hard"));
	for (i = 0; i<40; i++) {
		gotoxy( i / 20 * 20 + x-3, 2 + i % 20 + y);
		fscanf(Rbuf, "%s %d", name, &score);
		printf("%02d) %-20s %5d", i + 1, name, score);
	}
	for (i = 0; i<COLS / 2; i++) {
		gotoxy(i, y + 1);
		printf("━");
	}
	for (i = 0; i<COLS / 2; i++) {
		gotoxy(i, y + 22);
		printf("━");
	}
	for (i = 0; i<22; i++) {
		gotoxy(COLS / 8 + x, LINE / 4 + i + 1);
		printf("│");
	}
	gotoxy(COLS / 8 + x, LINE / 4 + 1); printf("┳"); gotoxy(COLS / 8 + x, LINE / 4 + i); printf("┻");
	fclose(Rbuf);
	gotoxy(COLS / 8 + 2, LINE / 4 + 23); printf("back to main");
	int p;
	int count = 50;
	while (kbhit()) getch();
	while (1) {
		if (count % 100 == 0) {
			gotoxy(COLS / 8 + 1, LINE / 4 + 23); printf("  ");
		}
		else if ((count % 100 - 50) == 0) {
			gotoxy(COLS / 8 + 1, LINE / 4 + 23); printf(" >");
		}
		count++;
		if (kbhit()) {
			p = getch();
			if (p == 224) p = getch();
			else if (p == ENTER) {
				gotoxy(COLS / 8 + 1, LINE / 4 + 23); printf("              ");
				return;
			}
		}
		Sleep(10);
	}
}
void option() {
	system("cls");
	gotoxy(COLS / 8 + 2, LINE / 4 + 3); printf("Coming Soon");
	gotoxy(COLS / 8 + 2, LINE / 4 + 23); printf("back to main");
	int p;
	int count = 50;
	while (1) {
		if (count % 100 == 0) {
			gotoxy(COLS / 8 + 1, LINE / 4 + 23); printf("  ");
		}
		else if ((count % 100 - 50) == 0) {
			gotoxy(COLS / 8 + 1, LINE / 4 + 23); printf(" >");
		}
		count++;
		if (kbhit()) {
			p = getch();
			if (p == 224) p = getch();
			else if (p == ENTER) {
				gotoxy(COLS / 8 + 1, LINE / 4 + 23); printf("              ");
				return;
			}
		}
		Sleep(10);
	}
}
void credits() {
	system("cls");
	gotoxy(COLS / 8 + 2, LINE / 4 + 3); printf("made by HIS");
	gotoxy(COLS / 8 + 2, LINE / 4 + 23); printf("back to main");
	int p;
	int count = 50;
	while (1) {
		if (count % 100 == 0) {
			gotoxy(COLS / 8 + 1, LINE / 4 + 23); printf("  ");
		}
		else if ((count % 100 - 50) == 0) {
			gotoxy(COLS / 8 + 1, LINE / 4 + 23); printf(" >");
		}
		count++;
		if (kbhit()) {
			p = getch();
			if (p == 224) p = getch();
			else if (p == ENTER) {
				gotoxy(COLS / 8 + 1, LINE / 4 + 23); printf("              ");
				return;
			}
		}
		Sleep(10);
	}
}
int EndGame() {
	int x = 40, y = 25;

	gotoxy(mapX + x, mapY + y); 	printf("┏------------------------┓ ");
	gotoxy(mapX + x, mapY + y + 1); printf("┃      Game  over!       ┃ ");
	gotoxy(mapX + x, mapY + y + 2); printf("┃                        ┃ ");
	gotoxy(mapX + x, mapY + y + 3); printf("┃       Retry            ┃ ");
	gotoxy(mapX + x, mapY + y + 4); printf("┃       Back to main     ┃ ");
	gotoxy(mapX + x, mapY + y + 5); printf("┃       Exit             ┃ ");
	gotoxy(mapX + x, mapY + y + 6); printf("┗------------------------┛ ");
	int p;
	int ch = 0;
	while (1) {
		gotoxy(mapX + x + 3, mapY + ch + y + 3);
		printf(" >");
		p = getch();
		gotoxy(mapX + x + 3, mapY + ch + y + 3);
		printf("  ");
		if (p == 224) {
			p = getch();
			if (p == UP) ch = (ch + 2) % 3;
			if (p == DOWN) ch = (ch + 1) % 3;
		}
		else if (p == ENTER) {
			gotoxy(mapX + x + 4, mapY + ch + y + 3);
			printf(" >");
			return ch;
		}
	}
}
void gotoxy(int x, int y) {
	COORD pos = { 2 * x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
void setcursortype(CURSOR_TYPE c) {
	CONSOLE_CURSOR_INFO CurInfo;
	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}