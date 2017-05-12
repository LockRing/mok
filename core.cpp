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

ordered_pair way[8] = { {0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1} };
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

bool open_check(int i, int j, int k, int num, bool is_m) {
	if ((i + way[k].y * num) < 0 || (i + way[k].y * num) >= MAXXY || (j + way[k].x * num) < 0 || (j + way[k].x * num) >= MAXXY)
	{
		return false;
	}
	
	if (is_m) {
		if (map[i + way[k].y * num][j + way[k].x * num] == enemy_color) {
			return false;
		}
	}
	else {
		if (map[i + way[k].y * num][j + way[k].x * num] == my_color) {
			return false;
		}
	}
	return true;
}

ordered_pair check_one() {
	ordered_pair ret;
	ret.x = -1;
	ret.y = -1;

	int i = 0;
	int j = 0;
	for (; i < MAXXY; ++i) {
		j = 0;
		for (; j < MAXXY; ++j) {

			if (my_status[i][j].is_check == true) {

				int k = 0;
				for (; k < 8; ++k) {
					if (my_status[i][j].way[k] >= 5 || my_status[i][j].way[k] + my_status[i][j].way[(k + 4) % 8] > 5) {
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
		j = 0;
		for (; j<MAXXY; ++j) {
			if (enemy_status[i][j].is_check == true) {
				int k = 0;
				for (; k<8; ++k) {
					if (enemy_status[i][j].way[k] >= 5 || enemy_status[i][j].way[k] + enemy_status[i][j].way[(k + 4) % 8] > 5) {
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
	int i,j;
	int check_max = 0;
	for (i = 0; i < MAXXY; i++) {
		for (j = 0; j < MAXXY; j++) {
			if (my_status[i][j].is_check == true)
			{
				{
					int k = 0;
					int count = 0;
					int open = 0;
					for (; k < 8; ++k)
					{
						if (my_status[i][j].way[k] == 4)
						{
							++count;
							if (open_check(i, j, k, 4, true))
							{
								++open;
							}

							if ((i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != enemy_color)
							{
								++open;
							}
						}

						if (count > 1 && open > 1)
						{
							return{ j,i };
						}
					}
				}

				{
					int k = 0;
					int count = 0;
					int open = 0;
					for (; k < 8; ++k)
					{
						if (my_status[i][j].way[k] == 4 || (my_status[i][j].way[k] == 3 && my_status[i][j].way[(k + 4) % 8] == 2))
						{
							++count;

							if (my_status[i][j].way[k] == 4 && open_check(i, j, k, 4, true))
							{
								++open;
							}
							if (my_status[i][j].way[k] == 4 && (i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != enemy_color)
							{
								++open;
							}
							if (my_status[i][j].way[k] == 3 && open_check(i, j, k, 3, true))
							{
								++open;
								if (open_check(i, j, (4 + k) % 8, 2, true))
								{
									++open;
								}
							}

							if (count > 1 && open > 1)
							{
								return{ j,i };
							}
						}
					}
				}
				/*
				{
				int k = 0;
				int count = 0;
				int open = 0;
				for (; k < 4; ++k)
				{
				if (my_status[i][j].way[k] == 3 && my_status[i][j].way[(k + 4) % 8] == 2)
				{
				++count;
				if (open_check(i, j, k, 3, true))
				{
				++open;
				}
				if (open_check(i, j, (k + 4) % 8, 2, true))
				{
				++open;
				}

				if (count > 1 && open > 1)
				{
				return {j,i};
				}
				}
				}
				}
				*/
			}
		}
	}
	return { -1,-1 };
}

ordered_pair check_four()
{
	int i = 0;
	int j = 0;
	for (; i < MAXXY; ++i)
	{
		j = 0;
		for (; j < MAXXY; ++j)
		{
			if (my_status[i][j].is_check == true)
			{
				int k = 0;
				for (; k < 8; ++k)
				{
					if (my_status[i][j].way[k] == 4 && open_check(i, j, k, 4, true) && (i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != enemy_color)
					{
						return {j,i};
					}
					
					if (my_status[i][j].way[k] == 3 && my_status[i][j].way[(4 + k) % 8] == 2 && open_check(i, j, k, 3, true) && open_check(i, j, (4 + k) % 8, 2, true))
					{
						return {j,i};
					}
				}
			}
		}
	}
	return {-1,-1};
}

ordered_pair check_five()
{
	int i = 0;
	int j = 0;
	for(;i<MAXXY;++i)
	{
		j = 0;
		for(;j<MAXXY;++j)
		{
			if(my_status[i][j].is_check == true)
			{
				int k = 0;
				for(;k<8;++k)
				{
					if (my_status[i][j].way[k] == 4 && (i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != enemy_color)
					{
						return {j,i};
					}
				}
			}
		}
	}
	return {-1,-1};
}

ordered_pair check_six() {//적4.4막기
	int i, j;
	for (i = 0; i < MAXXY; i++) {
		for (j = 0; j < MAXXY; j++) {
			if (enemy_status[i][j].is_check == true)
			{
				{
					int k = 0;
					int count = 0;
					int open = 0;
					for (; k < 8; ++k)
					{
						if (enemy_status[i][j].way[k] == 4)
						{
							++count;
							if (open_check(i, j, k, 4, false))
							{
								++open;
							}

							if ((i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != my_color)
							{
								++open;
							}
						}

						if (count > 1 && open > 1)
						{
							return {j,i};
						}
					}
				}

				{
					int k = 0;
					int count = 0;
					int open = 0;
					for (; k < 8; ++k)
					{
						if (enemy_status[i][j].way[k] == 4 || (enemy_status[i][j].way[k] == 3 && enemy_status[i][j].way[(k + 4) % 8] == 2))
						{
							++count;

							if (enemy_status[i][j].way[k] == 4 && open_check(i, j, k, 4, false))
							{
								++open;
							}
							if (enemy_status[i][j].way[k] == 4 && (i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != my_color)
							{
								++open;
							}
							if (enemy_status[i][j].way[k] == 3 && open_check(i, j, k, 3, false))
							{
								++open;
								if (open_check(i, j, (4 + k) % 8, 2, false))
								{
									++open;
								}
							}

							if (count > 1 && open > 1)
							{
								return{ j,i };
							}
						}
					}
				}
				/*
				{
					int k = 0;
					int count = 0;
					int open = 0;
					for (; k < 4; ++k)
					{
						if (enemy_status[i][j].way[k] == 3 && enemy_status[i][j].way[(k + 4) % 8] == 2)
						{
							++count;
							if (open_check(i, j, k, 3, false))
							{
								++open;
							}
							if (open_check(i, j, (k + 4) % 8, 2, false))
							{
								++open;
							}

							if (count > 1 && open > 1)
							{
								return {j,i};
							}
						}
					}
				}
				*/
			}
		}
	}
	return { -1,-1 };
}

ordered_pair check_seven() {//4.3
	int i, j;
	for (i = 0; i < MAXXY; i++) {
		for (j = 0; j < MAXXY; j++) {
			if (my_status[i][j].is_check == true)
			{
				{
					int k = 0;
					int count3 = 0;
					int count4 = 0;
					int open = 0;
					for (; k < 8; ++k)
					{
						if (my_status[i][j].way[k] == 4)
						{
							++count4;
							if (open_check(i, j, k, 4, true))
							{
								++open;
								if ((i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != enemy_color)
								{
									return {j,i};
								}
							}
							if ((i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != enemy_color)
							{
								++open;
							}
						}
						if (my_status[i][j].way[k] == 3)
						{
							++count3;
							if (open_check(i, j, k, 2, true))
							{
								++open;
							}
							if ((i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != enemy_color)
							{
								++open;
							}
						}

						if (count4 > 0 && count3 > 0 && open > 2)
						{
							return {j,i};
						}
					}
				}

				{
					int k = 0;
					int count3 = 0;
					int count32 = 0;
					int open = 0;
					for (; k < 8; ++k)
					{
						if (my_status[i][j].way[k] == 3 && my_status[i][j].way[(4 + k) % 8] == 2)
						{
							++count32;
							if (open_check(i, j, k, 3, true))
							{
								++open;
								if (open_check(i,j,(4 + k) % 8,2,true))
								{
									return { j,i };
								}
							}
							if (open_check(i, j, (4 + k) % 8, 2, true))
							{
								++open;
							}
						}
						else if (my_status[i][j].way[k] == 3)
						{
							++count3;
							if (open_check(i, j, k, 3, true))
							{
								++open;
							}
							if ((i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != enemy_color)
							{
								++open;
							}
						}

						if (count32 > 0 && count3 > 0 && open > 2)
						{
							return {j,i};
						}
					}
				}

				{
					int k = 0;
					int count4 = 0;
					int count11 = 0;
					int open = 0;
					for (; k < 8; ++k)
					{
						if (my_status[i][j].way[k] == 4)
						{
							++count4;
							if (open_check(i, j, k, 4, true))
							{
								++open;
								if ((i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != enemy_color)
								{
									return{ j,i };
								}
							}
							if ((i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != enemy_color)
							{
								++open;
							}
						}

						if (k < 4)
						{
							if (my_status[i][j].way[k] == 2 && my_status[i][j].way[(4 + k) % 8] == 2)
							{
								++count11;
								if (open_check(i, j, k, 2, true))
								{
									++open;
								}
								if (open_check(i, j, (4 + k) % 8, 2, true))
								{
									++open;
								}
							}
						}
						if (count4 > 0 && count11 > 0 && open > 2)
						{
							return {j,i};
						}
					}
				}
			}
		}
	}

	return{ -1,-1 };
}

ordered_pair check_eight() {
	int i = 0;
	int j = 0;
	for (; i < MAXXY; ++i)
	{
		j = 0;
		for (; j < MAXXY; ++j)
		{
			if (enemy_status[i][j].is_check == true)
			{
				int k = 0;
				for (; k < 8; ++k)
				{
					if (enemy_status[i][j].way[k] == 4 && open_check(i, j, k, 4, false) && (i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != my_color)
					{
						return{ j,i };
					}

					if (enemy_status[i][j].way[k] == 3 && enemy_status[i][j].way[(4 + k) % 8] == 2 && open_check(i, j, k, 3, false) && open_check(i, j, (4 + k) % 8, 2, false))
					{
						return{ j,i };
					}
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
		j = 0;
		for (; j<MAXXY; ++j)
		{
			if (my_status[i][j].is_check == true)
			{
				{
					int k = 0;
					int count = 0;
					int open = 0;
					for (; k < 8; ++k)
					{
						if (my_status[i][j].way[k] == 3)
						{
							++count;
							if (open_check(i, j, k, 3, true))
							{
								++open;
							}
							if ((i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != enemy_color)
							{
								++open;
							}
						}
						if (count > 1 && open > 3)
						{
							return {j,i};
						}
					}
				}

				{
					int k = 0;
					int count = 0;
					for (; k < 4; ++k)
					{
						if (my_status[i][j].way[k] == 2 && my_status[i][j].way[(4 + k) % 8] == 2 && open_check(i, j, k, 2, true) && open_check(i, j, (4 + k) % 8, 2, true))
						{
							++count;
						}

						if (count > 1)
						{
							return { j,i };
						}
					}
				}

				{
					int k = 0;
					int count3 = 0;
					int count11 = 0;
					int open = 0;
					for (; k < 8; ++k)
					{
						if (my_status[i][j].way[k] == 3)
						{
							++count3;
							if (open_check(i, j, k, 3, true))
							{
								++open;
							}
							if ((i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != enemy_color)
							{
								++open;
							}
						}
						if (k < 4)
						{
							if (my_status[i][j].way[k] == 2 && my_status[i][j].way[(4 + k) % 8] == 2)
							{
								++count11;
								if (open_check(i, j, k, 2, true))
								{
									++open;
								}
								if (open_check(i, j, (4 + k) % 8, 2, true))
								{
									++open;
								}
							}
						}

						if (count3 > 0 && count11 > 0 && open > 3)
						{
							return {j,i};
						}
					}
				}

				{

				}
			}

			if (map[i][j] == EMPTY)
			{
				int count = 0;
				if (my_status[i][j].is_check == true)
				{
					int k = 0;
					for (; k < 8; ++k)
					{
						if (my_status[i][j].way[k] == 3 && open_check(i, j, k, 3, true) && open_check(i, j, (4 + k) % 8, 1, true))
						{
							++count;
						}
						else if (my_status[i][j].way[k] == 1 && my_status[i + way[k].y][j + way[k].x].is_check == true && my_status[i + way[k].y][j + way[k].x].way[k] == 3 && open_check(i + way[k].y, j + way[k].x, k, 3, true) && open_check(i, j, (4 + k) % 8, 1,true))
						{
							++count;
						}
					}

				}
				else
				{
					int k = 0;
					for (; k < 8; ++k)
					{
						if (my_status[i + way[k].y][j + way[k].x].is_check == true)
						{
							if (my_status[i + way[k].y][j + way[k].x].way[k] == 3 && open_check(i + way[k].y, j + way[k].x, k, 3, true) && open_check(i, j, (4 + k) % 8, 1, true))
							{
								++count;
							}
						}
					}
				}

				if (count > 1)
				{
					return{ j,i };
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
		j = 0;
		for (; j<MAXXY; ++j)
		{
			if (enemy_status[i][j].is_check == true)
			{
				{
					int k = 0;
					int count = 0;
					int open = 0;
					for (; k < 8; ++k)
					{
						if (enemy_status[i][j].way[k] == 3)
						{
							++count;
							if (open_check(i, j, k, 3, false))
							{
								++open;
							}
							if ((i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != my_color)
							{
								++open;
							}
						}
						if (count > 1 && open > 3)
						{
							return{ j,i };
						}
					}
				}

				{
					int k = 0;
					int count = 0;
					for (; k < 4; ++k)
					{
						if (enemy_status[i][j].way[k] == 2 && enemy_status[i][j].way[(4 + k) % 8] == 2 && open_check(i, j, k, 2, false) && open_check(i, j, (4 + k) % 8, 2, false))
						{
							++count;
						}
						if (count > 1)
						{
							return {j,i};
						}
					}
				}

				{
					int k = 0;
					int count3 = 0;
					int count11 = 0;
					int open = 0;
					for (; k < 8; ++k)
					{
						if (enemy_status[i][j].way[k] == 3)
						{
							++count3;
							if (open_check(i, j, k, 3, false))
							{
								++open;
							}
							if ((i - way[k].y) >= 0 && (i - way[k].y) < MAXXY && (j - way[k].x) >= 0 && (j - way[k].x) < MAXXY && map[i - way[k].y][j - way[k].x] != my_color)
							{
								++open;
							}
						}
						if (k < 4)
						{
							if (enemy_status[i][j].way[k] == 2 && enemy_status[i][j].way[(4 + k) % 8] == 2)
							{
								++count11;
								if (open_check(i, j, k, 2, false))
								{
									++open;
								}
								if (open_check(i, j, (4 + k) % 8, 2, false))
								{
									++open;
								}
							}
						}

						if (count3 > 0 && count11 > 0 && open > 3)
						{
							return{ j,i };
						}
					}
				}

				{

				}
			}

			if (map[i][j] == EMPTY)
			{
				int count = 0;
				if (enemy_status[i][j].is_check == true)
				{
					int k = 0;
					for (; k < 8; ++k)
					{
						if (enemy_status[i][j].way[k] == 3 && open_check(i, j, k, 3, false) && open_check(i,j,(4 + k) % 8,1,false))
						{
							++count;
						}
						else if (enemy_status[i][j].way[k] == 1 && enemy_status[i + way[k].y][j + way[k].x].is_check == true && enemy_status[i + way[k].y][j + way[k].x].way[k] == 3 && open_check(i + way[k].y,j + way[k].x, k ,3,false) && open_check(i,j,(4 + k) % 8,1,false))
						{
							++count;
						}
					}

				}
				else
				{
					int k = 0;
					for (; k < 8; ++k)
					{
						if (enemy_status[i + way[k].y][j + way[k].x].is_check == true)
						{
							if (enemy_status[i + way[k].y][j + way[k].x].way[k] == 3 && open_check(i + way[k].y, j + way[k].x, k, 3, false) && open_check(i,j,(4 + k) % 8,1,false))
							{
								++count;
							}
						}
					}
				}

				if (count > 1)
				{
					return {j,i};
				}
			}
		}
	}
	return ret;
}

ordered_pair check_eleven()
{
	int i = 0;
	int j = 0;
	for (; i<MAXXY; ++i)
	{
		j = 0;
		for (; j<MAXXY; ++j)
		{
			if (my_status[i][j].is_check == true)
			{
				int k = 0;
				for (; k<8; ++k)
				{
					if (my_status[i][j].way[k] == 3 && open_check(i, j, k, 3, true) && open_check(i,j,(4 + k) % 8,1,true))
					{
						return { j,i };
					}
				}
			}
		}
	}
	return {-1,-1};
}

ordered_pair check_twelve()
{
	int i = 0;
    int j = 0;
    for(;i<MAXXY;++i)
    {   
        for(;j<MAXXY;++j)
        {
            if(my_status[i][j].is_check == true)
            {
                
            }
        }
    }   
	return {-1,-1};
}

void get_map() {
	int i, j;
	for (i = 0; i < MAXXY; i++) {
		for (j = 0; j < MAXXY; j++) {
			if (B(j, i) != map[i][j]) {
				map[i][j] = B(j, i);
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

void update_status(int x, int y, bool me)
{
	int i = 0;
	for (; i<4; ++i)
	{
		if (me)
		{
			int ver = way[i].y;
			int ho = way[i].x;
			int c = 1;
			while (map[y + ver * c][x + ho * c] == my_color &&  (y +ver * c) >= 0 && (y + ver * c) < MAXXY &&  (x +ho * c) >= 0 && (x + ho * c) < MAXXY)
			{
				++c;
			}
			int d = 1;
			while (map[y - ver * d][x - ho * d] == my_color &&  (y -ver * d) >= 0 && (y - ver * d) < MAXXY &&  (x - ho * d) >= 0 && (x - ho * d) < MAXXY)
			{
				++d;
			}
			
			if (map[y + ver * c][x + ho * c] == EMPTY &&  (y +ver * c) >= 0 && (y + ver * c) < MAXXY &&  (x +ho * c) >= 0 && (x + ho * c) < MAXXY)
			{
				my_status[y + ver * c][x + ho * c].is_check = true;
				my_status[y + ver * c][x + ho * c].way[(4 + i) % 8] = c + d;
			}
			if (map[y - ver * d][x - ho * d] == EMPTY &&  (y - ver * d) >= 0 && (y - ver * d) < MAXXY &&  (x - ho * d) >= 0 && (x - ho * d) < MAXXY)
			{
				my_status[y - ver * d][x - ho * d].is_check = true;
				my_status[y - ver * d][x - ho * d].way[i] = c + d;
			}
		}
		else
		{
			int ver = way[i].y;
			int ho = way[i].x;
			int c = 1;
			while (map[y + ver * c][x + ho * c] == enemy_color &&  (y +ver * c) >= 0 && (y + ver * c) < MAXXY &&  (x +ho * c) >= 0 && (x + ho * c) < MAXXY)
			{
				++c;
			}
			int d = 1;
			while (map[y - ver * d][x - ho * d] == enemy_color &&  (y - ver * d) >= 0 && (y - ver * d) < MAXXY &&  (x - ho * d) >= 0 && (x - ho * d) < MAXXY)
			{
				++d;
			}

			if (map[y + ver * c][x + ho * c] == EMPTY &&  (y +ver * c) >= 0 && (y + ver * c) < MAXXY &&  (x +ho * c) >= 0 && (x + ho * c) < MAXXY)
			{
				enemy_status[y + ver * c][x + ho * c].is_check = true;
				enemy_status[y + ver * c][x + ho * c].way[(4 + i) % 8] = c + d;
			}
			if (map[y - ver * d][x - ho * d] == EMPTY &&  (y - ver * d) >= 0 && (y - ver * d) < MAXXY &&  (x - ho * d) >= 0 && (x - ho * d) < MAXXY)
			{
				enemy_status[y - ver * d][x - ho * d].is_check = true;
				enemy_status[y - ver * d][x - ho * d].way[i] = c + d;
			}
		}
	}
}

ordered_pair eval_weight() {
	ordered_pair ret = check_one();
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_two();
	if(ret.x > -1)
	{
		return ret;
	}
	ret = check_three();
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_four();
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_five();
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_six();
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_seven();
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_eight();
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_nine();
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_ten();
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_eleven();
	if (ret.x > -1)
	{
		return ret;
	}

	int i = 0;
	int j = 0;
	int chk = 0;
	for (; i < MAXXY; ++i)
	{
		j = 0;
		for (; j < MAXXY; ++j)
		{
			int k = 0;
			int here = 0;
			if (enemy_status[i][j].is_check == true)
			{
				for (; k < 8; ++k)
				{
					here += enemy_status[i][j].way[k];
				}
			}
			if (my_status[i][j].is_check == true)
			{
				for (k = 0; k < 8; ++k)
				{
					here += enemy_status[i][j].way[k];
				}
			}

			if (chk < here)
			{
				chk = here;
				ret.x = j;
				ret.y = i;
			}
		}
	}

	if (ret.x > -1)
	{
		return ret;
	}
	
	ret.x = rand() % MAXXY;
	ret.y = rand() % MAXXY;
	while (map[ret.y][ret.x] != EMPTY)
	{
		ret.x = rand() % MAXXY;
		ret.y = rand() % MAXXY;
	}
	return ret;
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
	ordered_pair ret;
	if (CurTurn == 0)
	{
		ret.x = 6;
		ret.y = 6;
	}
	else
	{
		get_map();

		update_status(enemy.x, enemy.y, false);

		ret = eval_weight();
	}

	/*
	if (CurTurn == 1)
		ret = { 2,3 };
	else if (CurTurn == 3)
		ret = { 4,3 };
	else if (CurTurn == 5)
		ret = { 3,4 };
	else if (CurTurn == 7)
		ret = { 3,5 };
	else if (CurTurn == 9)
	ret = { 5,4 };
	else if (CurTurn == 11)
	ret = { 5,1 };
	*/

	*NewX = ret.x;
	*NewY = ret.y;
	map[ret.y][ret.x] = my_color;
	my_status[ret.y][ret.x].is_check = false;
	enemy_status[ret.y][ret.x].is_check = false;
	update_status(ret.x,ret.y,true);
}
