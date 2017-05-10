#include<cstdio>
#include<iostream>
#include<cstdlib>
#include<Windows.h>
#include"201701001.h"

#define MAXXY 16
#define BLACK 1
#define WHITE 0
#define EMPTY -1
#define OUTBD -2

int dmap[MAXXY][MAXXY];

using namespace std;

void remove_cursor(void) {
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void gotoxy(int x, int y)//gotoxy
{
	COORD pos = { x - 1, y - 1 };//커서가 X좌표에서 -1 한값. Y좌표에서 -1한 값으로 이동
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void show_map(void) { //맵표현 gotoxy
	int i, j,k;
	gotoxy(1, 1);
	printf("\n");


	printf("┌");
	for (j = 0; j < MAXXY; j++) { //메인창 바닥
		printf("─");
	}
	printf("┐");
	printf("\n");

	for (i = 0; i <= MAXXY + 1; i++) { //메인창의 높이
		if (i != MAXXY + 1) {
			printf("┃"); //메인창 테두리
			for (j = 0; j < MAXXY; j++) { //메인창 너비
				if (dmap[i][j] == -1) {//빈칸
					int is_enemy = 0;
					if (enemy_status[i][j].is_check == true) {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
						int max = enemy_status[i][j].way[0];//가장큰 값 출력
						for (k = 0; k < 8; k++) {
							if (max < enemy_status[i][j].way[k]) {
								max = enemy_status[i][j].way[k];
							}
						}
						printf("%d", max);//숫자는 반각
						is_enemy++;
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					}
					if (my_status[i][j].is_check == true) {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
						int max = my_status[i][j].way[0];//가장큰 값 출력
						for (k = 0; k < 8; k++) {
							if (max < my_status[i][j].way[k]) {
								max = my_status[i][j].way[k];
							}
						}
						if (is_enemy) {
							printf("%d", max);//숫자는 반각
						}
						else {
							printf(" %d", max);
						}
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					}
					if (is_enemy == 0) {
						printf("  ");
					}
				}
				else if (dmap[i][j] == 1) {
					printf("●");//ㅁ한자에 있음
				}
				else {
					printf("○");
				}
			}
			printf("┃");// 테두리
			printf("\n");
		}
		else if (i == MAXXY + 1) {
			printf("└");
			for (j = 0; j < MAXXY; j++) { //메인창 바닥
				printf("─");
			}
			printf("┘");
			printf("\n");
		}

	}
}
int B(int x, int y, int def = OUTBD)
{
	if (x < 0 || x >= MAXXY) return def;
	if (y < 0 || y >= MAXXY) return def;
	return dmap[x][y];
}
bool IsGameOver(void)
{
	int iy, ix, n, d;
	int dx[] = { 1,1,0,-1 };
	int dy[] = { 0,1,1,1 };
	for (iy = 0; iy < MAXXY; iy++) {
		for (ix = 0; ix < MAXXY; ix++) {
			for (d = 0; d < 4; d++)
				if (B(ix, iy) != EMPTY) {
					for (n = 1; n < 5; n++)
						if (B(ix, iy) != B(ix + n*dx[d], iy + n*dy[d]))
							break;
					if (n == 5) return TRUE;
				}
		}
	}

	return FALSE;
}


int main(int argv, char* argc) {
	int x, y;
	int mc = BLACK;
	int curturn = 1;
	while (!IsGameOver) {
		f201701001(&x, &y, mc, curturn++);
		dmap[x][y] = mc;
		(mc == BLACK) ? WHITE : BLACK;
		show_map();
		Sleep(100);
	}
}
