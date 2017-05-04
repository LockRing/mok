#include "stdafx.h"
#include <cstring>

typedef struct ordered_pair {
	int x;
	int y;
}ordered_pair;
typedef struct status {
	char way[8];
	bool is_check;
}status;

#define MAXXY 16
#define BLACK 1
#define WHITE 0
#define EMPTY -1
#define OUTBD -2

int map[MAXXY][MAXXY];
int weight[MAXXY][MAXXY] = { 0, };
ordered_pair way[8] = { { 0,1 },{ 1,1 },{ 1,0 },{ 1,-1 },{ 0,-1 },{ -1,-1 },{ -1,0 },{ -1,1 } };
ordered_pair enemy;
ordered_pair my;
int my_color;
int enemy_color;

/*
my_status 와 enemy_status 를 기반으로
상황에 가중치를 줘서 weight에 집어넣어서
각 지점의 현제 착수가
얼마나 가치있는지 평가한다.
가중치는 나머지 모든 상황이 겹쳐도 막아야 되는 상황 (ex close4방어)
두가지 상황의 가중치가 가산되어서 더중요하게 된상황 (ex 3과 4를 동시에 만드는 경우)
이 차이있으므로 자릿수로 중요도의 단계를 구분해서
가중치를 주기로 한다.
*/
status my_status[MAXXY][MAXXY];
/* 읽어온 map으로 union find 평가 한다.
6789들은 weight에서 계산할 수도 있다.
매직넘버 자체를 가중치로 사용할 수도 있다.

0 : 빈칸

1 : close 1
2 : close 2
3 : close 3
4 : close 4

6 : close1->close2 point
7 : close2->close3 point
8 : close3->close4 point
9 : close4->close5 point

11 : open 1
22 : open 2
33 : open 3
44 : open 4

66 : open1->open2 point //8방위
77 : open2->open3 point //2군데
88 : open3->open4 point //2군데
99 : close4->close5 point//2군데 - > 어짜피 막았어야함. 근데 인공지능이니 일단 막기

-11 : 적
*/
status enemy_status[MAXXY][MAXXY];// 읽어온 map으로 union find 평가 한다.

int turn = 0;

extern int B(int x, int y, int def = OUTBD);
void init() {
	int i, j;
	for (i = 0; i < MAXXY; i++) {
		for (j = 0; j < MAXXY; j++) {
			map[i][j] = -1;
			memset(enemy_status[i][j].way, 1, 8);
			enemy_status[i][j].is_check = false;
			memset(my_status[i][j].way, 1, 8);
			my_status[i][j].is_check = false;
		}
	}
}
/*
get_status
enemy를 기준으로 8방위만 평가하면 된다.
*/
void get_enemy_status(void) {
	int c;
	ordered_pair way[8] = { {0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1} };
	for (c = 0; c < 8; c++) {
		int ho = way[c].y;
		int ver = way[c].x;
		if (map[enemy.y + ver][enemy.x + ho] == EMPTY) {
			enemy_status[enemy.y + ver][enemy.x + ho].way[c] += 1;
			enemy_status[enemy.y + ver][enemy.x + ho].is_check = true;
		}
		else {
			while (map[enemy.y + ver][enemy.x + ho] == enemy_color) {
				ver += way[c].y;
				ho += way[c].x;
			}

			if (map[enemy.y + ver][enemy.x + ho] == EMPTY) {
				enemy_status[enemy.y + ver][enemy.x + ho].way[c] += 1;
				enemy_status[enemy.y + ver][enemy.x + ho].is_check = true;
			}
		}


	}
}

bool open_check(int i, int j, int k, int num, bool is_m) {
	int l;
	int ho = 0;
	int ver = 0;
	for (l = 0; l < num; l++) {
		ho += way[k].y;
		ver += way[k].x;
	}
	if (is_m) {
		if (my_status[i + ver][j + ho].way[k] == num) {
			return true;
		}
	}
	else {
		if (enemy_status[i + ver][j + ho].way[k] == num) {
			return true;
		}
	}
	return false;
}

ordered_pair check_one() {
	ordered_pair ret;
	ret.x = -1;
	ret.y = -1;

	int i = 0;
	int j = 0;
	for (; i<MAXXY; ++i) {
		for (; j<MAXXY; ++j) {
			if (my_status[i][j].is_check == true) {
				int k = 0;
				for (; k<8; ++k) {
					if (my_status[i][j].way[k] == 5 || my_status[i][j].way[k] + my_status[i][j].way[(k + 4) % 8] > 5) {
						ret.x = j;
						ret.y = i;
						return ret;
					}
				}
			}
		}
	}
	return ret;
}

ordered_pair check_two() {
	ordered_pair ret;
	ret.x = -1;
	ret.y = -1;

	int i = 0;
	int j = 0;
	for (; i<MAXXY; ++i) {
		for (; j<MAXXY; ++j) {
			if (enemy_status[i][j].is_check == true) {
				int k = 0;
				for (; k<8; ++k) {
					if (enemy_status[i][j].way[k] == 5 || enemy_status[i][j].way[k] + enemy_status[i][j].way[(k + 4) % 8] > 5) {
						ret.x = j;
						ret.y = i;
						return ret;
					}
				}
			}
		}
	}
	return ret;
}

ordered_pair check_three()
{
ordered_pair ret;
ret.x = -1;
ret.y = -1;

int i = 0;
int j = 0;
for(;i<MAXXY;++i)
{
	for(;j<MAXXY;++j)
	{
		if(my_status[i][j].is_check == true)
		{
			int k = 0;
			int count4 = 0;
			int count3 = 0;
			int open_count4 = 0;
			int open_count3 = 0;
			for(;k<8;++k)
			{
				if(my_status[i][j].way[k] == 4)
				{
					++count4;
					if(open_check(i,j,k,4,true))
					{
						++open_count4;
					}
					if(map[i - way[k].y][j - way[k].x] != enemy_color)
					{
						++open_count4;
					}

				}
				else if(my_status[i][j].way[k] == 3)
				{
					++count3;
					if(my_status[i][j].way[(k + 4) % 8] > 1)
					{
						if(open_check(i,j,k,3,true))
						{
							++open_count3;
						}
						if(map[i - 2*(way[k].y)][j - 2*(way[k].x)] == EMPTY)
						{
							++open_count3;
						}
					}
				}

				if(count4 > 1 && open_count4 > 1)
				{
					ret.x = j;
					ret.y = i;
					return ret;
				}
				else if(count4 > 0 && count3 > 0 && open_count4 + open_count3 > 1)
				{
					ret.x = j;
					ret.y = i;
					return ret;				
				}
				else if(count3 > 1 && open_count3 > 1)
				{
					ret.x = i;
					ret.y = i;
					return ret;
				}
			}
		}
	}
}
return ret;
}

ordered_pair check_four2five()
{
	ordered_pair ret;
	ret.x = -1;
	ret.y = -1;

	int i = 0;
	int j = 0;
	for(;i<MAXXY;++i)
	{
		for(;j<MAXXY;++j)
		{
			if(my_status[i][j].is_check == true)
			{
				int k = 0;
				for(;k<8;++k)
				{
					if(my_status[i][j].way[k] == 4)
					{
						ret.x = j;
						ret.y = i;
						if(map[i - way[k].y][j - way[k].x] == EMPTY && open_check(i,j,k,4,true))
						{
							rerturn ret;
						}
						else if(map[i - way[k].y][j - way[k].x] == enemy_color && !open_check(i,j,k,4,true))
						{
							ret.x = -1;
							ret.y = -1;
						}
					}
				}
			}
		}
	}
	return ret;
}

ordered_pair three_to_five_check() {
	int i, j, k;
	int l;
	int check_max = 0;
	ordered_pair a;
	for (i = 0; i < MAXXY; i++) {
		for (j = 0; j < MAXXY; j++) {
			int check = 0;
			for (k = 0; k < 8; k++) {
				if (my_status[i][j].way[k] == 4 || my_status[i][j].way[k] + my_status[i][j].way[(k + 4) % 8] > 4) {
					if (open_check(i, j, k, 4, true)) {
						check++;
					}
					check++;
				}
			}
			if (check > check_max) {
				check_max = check;
				a = { j,i };
			}

		}
	}
	if (!check_max) {
		return{ -1,-1 };
	}
	return a;
}

ordered_pair check_six() {
	int i, j, k;
	int l;
	int check_max = 0;
	ordered_pair a;
	for (i = 0; i < MAXXY; i++) {
		for (j = 0; j < MAXXY; j++) {
			int check = 0;
			for (k = 0; k < 8; k++) {
				if (enemy_status[i][j].way[k] == 4 || my_status[i][j].way[k] + my_status[i][j].way[(k + 4) % 8] > 4) {
					if (open_check(i, j, k, 4, true)) {
						check++;
					}
					check++;
				}
			}
			if (check > check_max) {
				check_max = check;
				a = { j,i };
			}
		}
	}
	if (check_max < 2) {
		return{ -1,-1 };
	}
	else {
		return a;
	}
	return a;
}

ordered_pair check_seven() {//4.3
	int i, j, k;
	ordered_pair a;
	for (i = 0; i < MAXXY; i++) {
		for (j = 0; j < MAXXY; j++) {
			for (k = 0; k < 8; k++) {
				if (my_status[i][j].way[k] == 3 || my_status[i][j].way[k] + my_status[i][j].way[(k + 4) % 8] > 3) {
					while (k < 8) {
						k++;
						if (my_status[i][j].way[k] == 4 || my_status[i][j].way[k] + my_status[i][j].way[(k + 4) % 8] > 4) {
							return{ i,j };
						}
					}
				}
				if (my_status[i][j].way[k] == 4 || my_status[i][j].way[k] + my_status[i][j].way[(k + 4) % 8] > 4) {
					while (k < 8) {
						k++;
						if (my_status[i][j].way[k] == 3 || my_status[i][j].way[k] + my_status[i][j].way[(k + 4) % 8] > 3) {
							return{ i,j };
						}
					}
				}
			}
		}
	}

	return{ -1,-1 };
}

ordered_pair check_eight() {
	int i, j, k;
	int l;
	for (i = 0; i < MAXXY; i++) {
		for (j = 0; j < MAXXY; j++) {
			int check = 0;
			for (k = 0; k < 8; k++) {
				if (enemy_status[i][j].way[k] == 4 || my_status[i][j].way[k] + my_status[i][j].way[(k + 4) % 8] > 4) {
					if (open_check(i, j, k, 4, true)) {
						check++;
					}
					check++;
				}
				if (check >= 2) {
					return{ i,j };
				}
			}
		}
	}
	
	return{ -1,-1 };
}

ordered_pair check_nine()
{
	ordered_pair ret;
	ret.x = -1;
	ret.y = -1;

	int i = 0;
	int j = 0;
	for (; i<MAXXY; ++i)
	{
		for (; j<MAXXY; ++j)
		{
			if (my_status[i][j].is_check == true)
			{
				int k = 0;
				int count = 0;
				for (; k<8; ++k)
				{
					if (my_status[i][j].way[k] == 3 && open_check(i, j, k, 3, true) && map[i - way[k].y][j - way[k].x] != enemy_color && (map[i + 4*(way[k].y)][j + 4*(way[k].x)] != enemy_color || map[i - 2*(way[k].y)][j  - 2*(way[k].x)] != enemy_color))
					{
						if (count > 1)
						{
							ret.x = j;
							ret.y = i;
							return ret;
						}
					}
				}
			}
		}
	}
	return ret;
}

ordered_pair check_ten()
{
	ordered_pair ret;
	ret.x = -1;
	ret.y = -1;

	int i = 0;
	int j = 0;
	for (; i<MAXXY; ++i)
	{
		for (; j<MAXXY; ++j)
		{
			if (enemy_status[i][j].is_check == true)
			{
				int k = 0;
				int count = 0;
				for (; k<8; ++k)
				{
					if (enemy_status[i][j].way[k] == 3 && open_check(i, j, k, 3, false))
					{
						if (++count > 1)
						{
							ret.x = j;
							ret.y = i;
							return ret;
						}
					}
				}
			}
		}
	}
	return ret;
}

ordered_pair check_eleven()
{
	ordered_pair ret;
	ret.x = -1;
	ret.y = -1;

	int i = 0;
	int j = 0;
	for (; i<MAXXY; ++i)
	{
		for (; j<MAXXY; ++j)
		{
			if (my_status[i][j].is_check == true)
			{
				int k = 0;
				int count = 0;
				for (; k<8; ++k)
				{
					if (my_status[i][j].way[k] == 3 && open_check(i, j, k, 3, false))
					{
						if (map[i - way[k].y][j - way[k].x] != enemy_color)
						{
							ret.x = j;
							ret.y = i;
							return ret;
						}
						else
						{
							ret.x = j;
							ret.y = i;
						}
					}
				}
			}
		}
	}
	return ret;
}

ordered_pair check_twelve()
{
    ordered_pair ret;
    ret.x = -1; 
    ret.y = -1; 

    int i = 0;
    int j = 0;
    for(;i<MAXXY;++i)
    {   
        for(;j<MAXXY;++j)
        {
            if(my_status[i][j].is_check == true)
            {
                int k = 0;
		int level = 0;
                for(;k<8;++k)
                {
                    if(my_status[i][j].way[k] == 3)
                    {
                        if(map[i - way[k].y][j i- way[k].x] != enemy_color)
                        {
                            if(map[i - 2(way[k].y)][j - 2(way[k].x)] != enemy_color)
                            {
                                if(map[i -3(way[k].y)][j - 3(way[k].x)] != enemy_color)
                                {
                                    ret.x = j;
                                    ret.y = i;
                                    return ret;
                                }
                                else if(level < 3)
                                {
				    level = 3;
                                    ret.x = j;
                                    ret.y = i;
                                }
                            }
                            else if(level < 2)
                            {
				level = 2;
                                ret.x = j;
                                ret.y = i;
                            }
                        }
                        else if(level < 1)
                        {
			    level = 1;
                            ret.x = j;
                            ret.y = i;
                        }
                    }
                }
            }
        }
    }   
    return ret;
}

void get_my_status(void) {
	int c;

	for (c = 0; c < 8; c++) {
		int ho = way[c].y;
		int ver = way[c].x;
		if (map[my.y + ver][my.x + ho] == EMPTY) {
			enemy_status[my.y + ver][my.x + ho].way[c] += 1;
			enemy_status[my.y + ver][my.x + ho].is_check = true;
		}
		else {
			while (map[my.y + ver][my.x + ho] == enemy_color) {
				ver += way[c].y;
				ho += way[c].x;
			}

			if (map[my.y + ver][my.x + ho] == EMPTY) {
				enemy_status[my.y + ver][my.x + ho].way[c] += 1;
				enemy_status[my.y + ver][my.x + ho].is_check = true;
			}
		}


	}
}


void get_map() {
	int i, j;
	for (i = 0; i < MAXXY; i++) {
		for (j = 0; j < MAXXY; j++) {
			if (B(j, i) != map[i][j]) {
				map[i][j] = B(i, j);
				enemy = { j,i };
				my_status[enemy.y][enemy.x].is_check = false;
				enemy_status[enemy.y][enemy.x].is_check = false;
				return;
			}
		}
	}
}
/*
가중치(우선순위)
1. 첫수 -> 중앙지점 (8,8)에 놓는다.
2. 둘쩃수(상대방이 첫수인 경우) 상대방돌이 천원 근방 10안에 있으면 (3여유) 상대방에게 중앙 부분으로 붙이고
   아닌 경우 천원에 그리고 다음 수는 상대방에 중앙 부분으로 붙인다.
3. 내 4있을떄 -> 놓으면 승리
4. 상대방꺼 4개 연속 -> 무조건 막아야함
5. 44 -> 규칙상 44가능! -> 다음에 무조건 승리
6. 내꺼 양방향 4만들수 있을떄
7. 내꺼 단방향 4만들수 있을때
8. 상대방 양방향 3 -> 붙인다. 한쪽으로 ○●●●○◎ 상황 시 첫번쨰로 놓게 한다.->절약
9. 33 -> 규칙상 33가능! -> 다음에 무조건 승리 -> 만들어진 33 의 가중치를 높게 한다.
10. 22
9.
*/

 

ordered_pair eval_weight() {

}


void f201701001(int *NewX, int *NewY, int mc, int CurTurn)
{
	static bool is_init = false;
	if (!is_init) {
		init();
		is_init = true;
		my_color = mc;
		enemy_color = (mc == WHITE) ? BLACK : WHITE;
	}
	get_map();
	eval_weight();
	

}
