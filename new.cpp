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

static int map[MAXXY][MAXXY];

static ordered_pair way[8] = { { 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },{ -1,-1 } };

static int my_color;
static int enemy_color;

static status my_status[MAXXY][MAXXY];
static status enemy_status[MAXXY][MAXXY];

extern int B(int x, int y, int def = OUTBD);


ordered_pair check_three_three(status my_stat[][MAXXY], status enemy_stat[][MAXXY], int maps[][MAXXY], bool me);

void init() {
	int i, j;
	for (i = 0; i < MAXXY; i++) {
		for (j = 0; j < MAXXY; j++) {
			map[i][j] = -1;
			//memset(enemy_status[i][j].way, 1, 8);
			int k = 0;
			for (; k < 8; ++k)
			{
				enemy_status[i][j].way[k] = 1;
				my_status[i][j].way[k] = 1;
			}
			enemy_status[i][j].is_check = false;
			//memset(my_status[i][j].way, 1, 8);
			my_status[i][j].is_check = false;
		}
	}
}

bool open_check(int i, int j, int k, int num, int maps[][MAXXY], bool is_m) {
	if ((i + way[k].y * num) < 0 || (i + way[k].y * num) >= MAXXY || (j + way[k].x * num) < 0 || (j + way[k].x * num) >= MAXXY)
	{
		return false;
	}

	if (is_m) {
		if (maps[i + way[k].y * num][j + way[k].x * num] == enemy_color) {
			return false;
		}
	}
	else {
		if (maps[i + way[k].y * num][j + way[k].x * num] == my_color) {
			return false;
		}
	}
	return true;
}

ordered_pair get_map() {
	ordered_pair ret = { -1,-1 };
	int i, j;
	for (i = 0; i < MAXXY; i++) {
		for (j = 0; j < MAXXY; j++) {
			if (B(j, i) != map[i][j]) {
				map[i][j] = B(j, i);
				ret.x = j;
				ret.y = i;
				my_status[ret.y][ret.x].is_check = false;
				enemy_status[ret.y][ret.x].is_check = false;
				return ret;
			}
		}
	}
	return ret;
}

void update_status(int x, int y, status my_stat[][MAXXY], status enemy_stat[][MAXXY], int maps[][MAXXY], bool me)
{
	int chk_color = -1;
	status (*use_status)[MAXXY];
	if (me)
	{
		chk_color = my_color;
		use_status = my_stat;

	}
	else
	{
		chk_color = enemy_color;
		use_status = enemy_stat;
	}

	int i = 0;
	for (; i < 4; ++i)
	{
		int c = 1;
		int ver = y + c * way[i].y;
		int ho = x + c * way[i].x;

		while (ver >= 0 && ver < MAXXY && ho >= 0 && ver < MAXXY && maps[ver][ho] == chk_color)
		{
			++c;
			ver = y + c * way[i].y;
			ho = x + c * way[i].x;
		}
		int d = 1;
		ver = y - d * way[i].y;
		ho = x - d * way[i].x;
		while (ver >= 0 && ver < MAXXY && ho >= 0 && ver < MAXXY && maps[ver][ho] == chk_color)
		{
			++d;
			ver = y - d * way[i].y;
			ho = x - d * way[i].x;
		}

		ver = y + c * way[i].y;
		ho = x + c * way[i].x;
		if (maps[ver][ho] == EMPTY && ver >= 0 && ver < MAXXY && ho >= 0 && ho < MAXXY)
		{
			use_status[ver][ho].is_check = true;
			use_status[ver][ho].way[(4 + i) % 8] = c + d;
		}
		ver = y - d * way[i].y;
		ho = x - d * way[i].x;
		if(maps[ver][ho] == EMPTY && ver >= 0 && ver < MAXXY && ho >= 0 && ho < MAXXY)
		{
			use_status[ver][ho].is_check = true;
			use_status[ver][ho].way[i] = c + d;
		}
	}
}

ordered_pair check_five(status my_stat[][MAXXY],status enemy_stat[][MAXXY],int maps[][MAXXY],bool me) {
	status(*chk_status)[MAXXY];
	if (me)
	{
		chk_status = my_stat;
	}
	else
	{
		chk_status = enemy_stat;
	}

	int i = 0;
	int j = 0;
	for (; i < MAXXY; ++i) {
		j = 0;
		for (; j < MAXXY; ++j) {
			if (chk_status[i][j].is_check == true) {

				int k = 0;
				for (; k < 8; ++k) {
					if (chk_status[i][j].way[k] >= 5 || chk_status[i][j].way[k] + chk_status[i][j].way[(k + 4) % 8] > 5) {
						return { j,i };
					}
				}
			}
		}
	}
	return {-1,-1};
}

ordered_pair check_four_four(status my_stat[][MAXXY], status enemy_stat[][MAXXY], int maps[][MAXXY], bool me)
{
	status(*chk_status)[MAXXY];
	if (me)
	{
		chk_status = my_stat;
	}
	else
	{
		chk_status = enemy_stat;
	}

	int i = 0;
	int j = 0;
	for (; i < MAXXY; ++i)
	{
		j = 0;
			for (; j < MAXXY; ++j)
			{
				if (chk_status[i][j].is_check == true)
				{
					{// 1
						int k = 0;
						int count = 0;
						int open = 0;
						for (; k < 8; ++k)
						{
							if (chk_status[i][j].way[k] == 4)
							{
								++count;
								if (open_check(i, j, k, 4, maps, me))
								{
									++open;
								}

								if (open_check(i, j, (4 + k) % 8, 1, maps, me))
								{
									++open;
								}
							}

							if (count > 1 && open > 0)
							{
								return{ j,i };
							}
						}
					}

					{// 2
						int k = 0;
						int count = 0;
						int open = 0;
						for (; k < 8; ++k)
						{
							if (chk_status[i][j].way[k] == 4)
							{
								++count;

								if (open_check(i, j, k, 4, maps, me))
								{
									++open;
								}
								if (open_check(i,j,(4 + k) % 8,1,maps,me))
								{
									++open;
								}
								
							}
							else if (chk_status[i][j].way[k] == 3 && chk_status[i][j].way[(k + 4) % 8] == 2)
							{
								++count;
								if (open_check(i, j, k, 3, maps, me))
								{
									++open;
								}
								if (open_check(i, j, (4 + k) % 8, 2, maps, me))
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

					if (me == false)
					{
						int k;
						int count = 0;
						int open = 0;
						for (k = 0; k < 8; ++k)
						{
							if (chk_status[i][j].way[k] == 4 && !(open_check(i, j, k, chk_status[i][j].way[k], maps, me)))
							{
								++count;
								if (open_check(i, j, (4 + k) % 8, 1, maps, me))
								{
									++open;
								}
							}
							else if (chk_status[i + way[k].y][j + way[k].x].is_check == true && chk_status[i + way[k].y][j + way[k].x].way[k] == 4 && open_check(i + way[k].y, j + way[k].x, k, 4, maps, me))
							{
								++open;
								++count;
								if (open_check(i, j, (4 + k) % 8, 1, maps, me))
								{
									++open;
								}
							}

							if (count > 1 && open > 0)
							{
								return {j + way[k].x,i + way[k].y};
							}
						}
					}

					{// 3
						int k = 0;
						int count3d = 0;
						int count3c = 0;
						int count2_1 = 0;
						int count21 = 0;
						int open = 0;

						int fourcl = 0;
						int twoonecl = 0;
						for (; k < 8; ++k)
						{
							if (chk_status[i][j].way[k] >= 4)
							{
								++count3c;
								if (open_check(i, j, k, chk_status[i][j].way[k],maps, me))
								{
									++fourcl;
									++open;
								}
								if (open_check(i, j, (4 + k) % 8, 1, maps,me))
								{
									++fourcl;
									++open;
								}
							}
							else if (chk_status[i][j].way[k] >= 3 && chk_status[i - way[k].y][j - way[k].x].is_check == true && chk_status[i - way[k].y][j - way[k].x].way[(4 + k) % 8] >= 2)
							{
								++count2_1;
								if (open_check(i, j, k, chk_status[i][j].way[k], maps,me))
								{
									++open;
								}
								if (open_check(i - way[k].y, j - way[k].x, (4 + k) % 8,chk_status[i - way[k].y][j - way[k].x].way[(4 + k) % 8], maps,me))
								{
									++open;
								}
							}
							else if (chk_status[i + way[k].y][j + way[k].x].is_check == true && chk_status[i + way[k].y][j + way[k].x].way[k] >= 4)
							{
								++count3d;
								if (open_check(i + way[k].y, j + way[k].x, k, chk_status[i + way[k].y][j + way[k].x].way[k], maps,me))
								{
									++open;
								}
								if (open_check(i, j, (4 + k) % 8, 1, maps,me))
								{
									++open;
								}
							}
							else if(chk_status[i][j].way[k] >= 2 && chk_status[i - way[k].y][j - way[k].x].is_check == true && chk_status[i - way[k].y][j - way[k].x].way[(4 + k) % 8] >= 3)
							{
								++count2_1;
								if (open_check(i + way[k].y, j + way[k].x, k, chk_status[i + way[k].y][j + way[k].x].way[k], maps,me))
								{
									++open;
								}
								if (open_check(i, j, (4 + k) % 8, 1, maps,me))
								{
									++open;
								}
							}
							else if (chk_status[i][j].way[k] == 3 && chk_status[i][j].way[(4 + k) % 8] == 2)
							{
								++count21;
								if (open_check(i, j, k, 3, maps, me))
								{
									++twoonecl;
									++open;
								}
								if (open_check(i, j, (4 + k) % 8, 2, maps, me))
								{
									++twoonecl;
									++open;
								}
							}

							if (count3d > 1)
							{
								return{ j,i };
							}
							else if (count2_1 > 1)
							{
								return { j,i };
							}
							else if (count3d > 0 && count3c > 0 && fourcl > 0)
							{
								return {j,i};
							}
							else if (count3c > 0 && count2_1 > 0 && fourcl > 0)
							{
								return{ j,i };
							}
							else if (count3c > 0 && count21 > 0 && open > 1)
							{
								return{ j,i };
							}
							else if (count21 > 1 && open > 1)
							{
								return {j,i};
							}
							else if (count21 > 0 && count2_1 > 0 && twoonecl > 0)
							{
								return {j,i};
							}
						}
					}
				}
				else
				{
					{// 4
						if (maps[i][j] == EMPTY)
						{
							int k = 0;
							int count4c = 0;
							int count4d = 0;
							int open = 0;
							for (; k < 8; ++k)
							{
								if (chk_status[i][j].way[k] == 4)
								{
									++count4c;
									if (open_check(i, j, k, 4, maps,me))
									{
										++open;
									}
									if (open_check(i, j, (4 + k) % 8, 1, maps,me))
									{
										++open;
									}
								}
								else if (chk_status[i + way[k].y][j + way[k].x].is_check == true && chk_status[i + way[k].y][j + way[k].x].way[k] == 4)
								{
									++count4d;
									if (open_check(i + way[k].y, j + way[k].x, k, 4, maps,me))
									{
										++open;
									}
									if (open_check(i, j, (4 + k) % 8, 1, maps,me))
									{
										++open;
									}
								}

								if (count4d > 1)
								{
									return{ j,i };
								}
								else if (count4d > 0 && count4c > 0 && open > 0)
								{
									return{ j,i };
								}
							}
						}
					}
				}
			}
	}
	return {-1,-1};
}

ordered_pair check_open_four(status my_stat[][MAXXY], status enemy_stat[][MAXXY], int maps[][MAXXY], bool me)
{
	status(*chk_status)[MAXXY];
	if (me)
	{
		chk_status = my_stat;
	}
	else
	{
		chk_status = enemy_stat;
	}

	int i = 0;
	int j = 0;
	for (; i < MAXXY; ++i)
	{
		j = 0;
		for (; j < MAXXY; ++j)
		{
			if (chk_status[i][j].is_check == true)
			{
				int k = 0;
				for (; k < 8; ++k)
				{
					if (chk_status[i][j].way[k] == 4 && open_check(i, j, k, 4, maps,me) && open_check(i,j,(4 + k) % 8,1,maps,me))
					{
						if (me == false)
						{
							int c;
							for (c = 0; c < 8; ++c)
							{
								if ((i + way[c].y) >= 0 && (i + way[c].y) < MAXXY && (j + way[c].x) >=0 && (j + way[c].x) < MAXXY && map[i + way[c].y][j + way[c].x] == my_color)
								{
									return{j + 4 * way[k].x,i + 4 * way[k].y };
								}
							}
							return{ j,i };
						}
						return { j,i };
					}

					if (chk_status[i][j].way[k] == 3 && chk_status[i][j].way[(4 + k) % 8] == 2 && open_check(i, j, k, 3, maps,me) && open_check(i, j, (4 + k) % 8, 2, maps,me))
					{
						return{ j,i };
					}
				}
			}
		}
	}
	return { -1,-1 };
}

ordered_pair check_close_four(status my_stat[][MAXXY], status enemy_stat[][MAXXY], int maps[][MAXXY], bool me)
{
	status(*chk_status)[MAXXY];
	if (me)
	{
		chk_status = my_stat;
	}
	else
	{
		chk_status = enemy_stat;
	}

	int i = 0;
	int j = 0;
	for (; i<MAXXY; ++i)
	{
		j = 0;
		for (; j<MAXXY; ++j)
		{
			if (chk_status[i][j].is_check == true)
			{
				int k = 0;
				for (; k<8; ++k)
				{
					if (chk_status[i][j].way[k] == 4 && ( open_check(i - way[k].y,j - way[k].x,(4 + k) % 8,1,maps,me) || open_check(i, j, k, 4, maps, me)))
					{
						return {j,i};
					}
					else if (chk_status[i][j].way[k] == 3 && chk_status[i][j].way[(4 + k) % 8] == 2 && (open_check(i, j, k, 3, maps, me) || (open_check(i, j, (4 + k) % 8, 2, maps, me))))
					{
						return {j,i};
					}
				}
			}
		}
	}
	return {-1,-1};
}

ordered_pair check_four_three(status my_stat[][MAXXY], status enemy_stat[][MAXXY], int maps[][MAXXY], bool me)
{
	status(*chk_status)[MAXXY];
	if (me)
	{
		chk_status = my_stat;
	}
	else
	{
		chk_status = enemy_stat;
	}

	int i, j;
	for (i = 0; i < MAXXY; i++) {
		for (j = 0; j < MAXXY; j++) {
			if (chk_status[i][j].is_check == true)
			{
				{
					int k = 0;
					int count3 = 0;
					int count4 = 0;
					int open = 0;
					for (; k < 8; ++k)
					{
						if (chk_status[i][j].way[k] == 4)
						{
							++count4;
							if (open_check(i, j, k, 4, maps,me))
							{
								++open;
							}
							if (open_check(i, j, (4 + k) % 8, 1, maps, me))
							{
								++open;
							}
						}
						if (chk_status[i][j].way[k] == 3)
						{
							++count3;
							if (open_check(i, j, k, 2, maps,me))
							{
								++open;
							}
							if (open_check(i, j, (4 + k) % 8, 1, maps, me))
							{
								++open;
							}
						}

						if (count4 > 0 && count3 > 0 && open > 2)
						{
							return{ j,i };
						}
					}
				}

				{
					int k = 0;
					int count11 = 0;
					int count32 = 0;
					int open = 0;
					for (; k < 8; ++k)
					{
						if (chk_status[i][j].way[k] == 3 && chk_status[i][j].way[(4 + k) % 8] == 2)
						{
							++count32;
							if (open_check(i, j, k, 3, maps,me))
							{
								++open;
							}
							if (open_check(i, j, (4 + k) % 8, 2, maps,me))
							{
								++open;
							}
						}
						if (k < 4 && chk_status[i][j].way[k] == 2 && chk_status[i][j].way[(4 + k) % 8] == 2)
						{
							++count11;
							if (open_check(i, j, k, 2, maps,me))
							{
								++open;
							}
							if (open_check(i, j, (4 + k) % 8, 2, maps,me))
							{
								++open;
							}
						}

						if (count32 > 0 && count11 > 0 && open > 2)
						{
							return{ j,i };
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
						if (chk_status[i][j].way[k] == 3 && chk_status[i][j].way[(4 + k) % 8] == 2)
						{
							++count32;
							if (open_check(i, j, k, 3, maps,me))
							{
								++open;
							}
							if (open_check(i, j, (4 + k) % 8, 2, maps,me))
							{
								++open;
							}
						}
						else if (chk_status[i][j].way[k] == 3)
						{
							++count3;
							if (open_check(i, j, k, 3, maps,me))
							{
								++open;
							}
							if (open_check(i,j,(4 + k) % 8,1,maps,me))
							{
								++open;
							}
						}

						if (count32 > 0 && count3 > 0 && open > 2)
						{
							return{ j,i };
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
						if (chk_status[i][j].way[k] == 4)
						{
							++count4;
							if (open_check(i, j, k, 4, maps,me))
							{
								++open;
							}
							if (open_check(i, j, (4 + k) % 8, 1, maps, me))
							{
								++open;
							}
						}

						if (k < 4)
						{
							if (chk_status[i][j].way[k] == 2 && chk_status[i][j].way[(4 + k) % 8] == 2)
							{
								++count11;
								if (open_check(i, j, k, 2, maps,me))
								{
									++open;
								}
								if (open_check(i, j, (4 + k) % 8, 2, maps,me))
								{
									++open;
								}
							}
						}
						if (count4 > 0 && count11 > 0 && open > 2)
						{
							return{ j,i };
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
						if (chk_status[i][j].way[k] == 4)
						{
							++count4;
							if (open_check(i, j, k, 4, maps,me))
							{
								++open;
							}
							if (open_check(i, j, (4 + k) % 8, 1, maps,me))
							{
								++open;
							}
						}
						else if (chk_status[i][j].way[k] == 2 && chk_status[i - way[k].y][j - way[k].x].is_check == true && chk_status[i - way[k].y][j - way[k].x].way[(4 + k) % 8] == 2)
						{
							++count11;
							if (open_check(i, j, k, 2, maps, me))
							{
								++open;
							}
							if (open_check(i - way[k].y, j - way[k].x, (4 + k) % 8, 2, maps, me))
							{
								++open;
							}
						}

						if (count4 > 0 && count11 > 0 && open > 1)
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
					if (chk_status[i][j].way[k] == 4)
					{
						++count;
						if (open_check(i, j, k, 4, maps, me))
						{
							++open;
						}
					}
					else if (chk_status[i + way[k].y][j + way[k].x].is_check == true && chk_status[i + way[k].y][j + way[k].x].way[k] == 3)
					{
						++count;
						if (open_check(i + way[k].y, j + way[k].x, k, 3, maps, me))
						{
							++open;
						}
					}

					if (count > 1 && open > 2)
					{
						return{ j,i };
					}
				}
				}
			}
		}
	}

	return{ -1,-1 };
}

ordered_pair check_three_three(status my_stat[][MAXXY], status enemy_stat[][MAXXY], int maps[][MAXXY], bool me)
{
	status(*chk_status)[MAXXY];
	if (me)
	{
		chk_status = my_stat;
	}
	else
	{
		chk_status = enemy_stat;
	}

	int i = 0;
	int j = 0;
	for (; i < MAXXY; ++i)
	{
		j = 0;
		for (; j < MAXXY; ++j)
		{
			if (chk_status[i][j].is_check == true)
			{

				{

					int k = 0;
					int tmpk = -1;
					int count = 0;
					for (; k < 8; ++k)
					{
						if (chk_status[i][j].way[k] == 3 && open_check(i, j, (4 + k) % 8, 1, maps, me) && open_check(i, j, k, 3, maps, me))
						{
							if (tmpk < 0)
							{
								tmpk = k;
								++count;
							}
							else if (tmpk != (k + 4) % 8)
							{
								++count;
							}
						}
						else if (k < 4 && chk_status[i][j].way[k] == 2 && chk_status[i][j].way[(4 + k) % 8] == 2 && open_check(i, j, k, 2, maps, me) && open_check(i, j, (4 + k) % 8, 2, maps, me))
						{
							if (tmpk < 0)
							{
								tmpk = k;
								++count;
							}
							else if (tmpk != (k + 4) % 8)
							{
								++count;
							}
						}
						else if (chk_status[i + way[k].y][j + way[k].x].is_check == true && chk_status[i + way[k].y][j + way[k].x].way[k] == 3 && open_check(i + way[k].y, j + way[k].x, k, 3, maps, me) && open_check(i, j, (4 + k) % 8, 1, maps, me))
						{
							if (tmpk < 0)
							{
								tmpk = k;
								++count;
							}
							else if (tmpk != (k + 4) % 8)
							{
								++count;
							}
						}
						else if (chk_status[i][j].way[k] == 2 && chk_status[i - way[k].y][j - way[k].x].is_check == true && chk_status[i - way[k].y][j - way[k].x].way[(4 + k) % 8] == 2 && open_check(i, j, k, 2, maps, me) && open_check(i - way[k].y, j - way[k].x, (4 + k) % 8, 2, maps, me))
						{
							if (tmpk < 0)
							{
								tmpk = k;
								++count;
							}
							else if (tmpk != (k + 4) % 8)
							{
								++count;
							}
						}
						else if (chk_status[i][j].way[k] == 2 && open_check(i, j, k, 2, maps, me) && chk_status[i + 2 * way[k].y][j + 2 * way[k].x].is_check == true && chk_status[i + 2 * way[k].y][j + 2 * way[k].x].way[k] == 2 && open_check(i + 2 * way[k].y, j + 2 * way[k].x, k, 2, maps, me) && open_check(i, j, (4 + k) % 8, 1, maps, me))
						{
							if (tmpk < 0)
							{
								tmpk = k;
								++count;
							}
							else if (tmpk != (k + 4) % 8)
							{
								++count;
							}
						}

						if (count > 1)
						{
							return{ j,i };
						}
					}
				}

					{
						int k;
						for (k = 0; k < 8; ++k)
						{
							if (k % 2 == 0 && chk_status[i + way[k].y][j + way[k].x].is_check == true && chk_status[i + way[k].y][j + way[k].x].way[k] == 2 && open_check(i + way[k].y, j + way[k].x, k, 2, maps, me) && open_check(i, j, (4 + k) % 8, 1, maps, me))
							{
								
								int c2 = 0;
								int c2k = -1;
								int c1 = 0;
								int c1k = -1;
								if (chk_status[i][j].way[(k - 1 + 8) % 8] == 2 && open_check(i + way[(k -1 + 8) % 8].y, j + way[(k -1 + 8) % 8].x, (k - 2 + 8) % 8, 1, maps, me))
								{
									++c1;
									c1k = (k - 1 + 8) % 8;
								}
								if (chk_status[i][j].way[(k + 1 + 8) % 8] == 2 && open_check(i + way[(k + 1 + 8) % 8].y, j + way[(k + 1 + 8) % 8].x, (k + 2 + 8) % 8, 1, maps, me))
								{
									++c1;
									c1k =( k + 1 + 8) % 8;
								}
								if (chk_status[i][j].way[(k - 1 + 8) % 8] == 3 && open_check(i, j, (k - 1 + 8) % 8, 3, maps, me) && open_check(i, j, (3 + k) % 8, 1, maps, me) && open_check(i + way[(k - 1 + 8) % 8].y, j + way[(k - 1 + 8) % 8].x, (k - 2 + 8) % 8, 1, maps, me))
								{
									++c2;
									c2k = (k - 1 + 8) % 8;
								}
								if (chk_status[i][j].way[(k + 1 + 8) % 8] == 3 && open_check(i, j, (k + 1 + 8) % 8, 3, maps, me) && open_check(i, j, (5 + k) % 8, 1, maps, me) && open_check(i + way[(k + 1 + 8) % 8].y, j + way[(k + 1 + 8) % 8].x, (k + 2 + 8) % 8, 1, maps, me))
								{
									++c2;
									c2k = (k + 1 + 8) % 8;
								}
								if (c1 > 0 && c2 > 0)
								{
									return {j,i};
								}
							}
						}
					

				}

			}

			if (maps[i][j] == EMPTY)
			{
				int k = 0;
				int count = 0;
				for(; k < 8; ++k)
				{

					if (chk_status[i + way[k].y][j + way[k].x].is_check == true && chk_status[i + way[k].y][j + way[k].x].way[k] == 3 && open_check(i + way[k].y, j + way[k].x, k, 3, maps, me) && open_check(i, j, (4 + k) % 8, 1, maps, me))
					{
						++count;
					}


					if (count > 1)
					{
						return {j,i};
					}
				}
			}
		}
	}

	return {-1,-1};
}

ordered_pair check_close_three(status my_stat[][MAXXY], status enemy_stat[][MAXXY], int maps[][MAXXY], bool me)
{
	status(*chk_status)[MAXXY];
	if (me)
	{
		chk_status = my_stat;
	}
	else
	{
		chk_status = enemy_stat;
	}

	int i = 0;
	int j = 0;
	for (; i<MAXXY; ++i)
	{
		j = 0;
		for (; j<MAXXY; ++j)
		{
			if (chk_status[i][j].is_check == true)
			{
				int k = 0;
				for (; k<8; ++k)
				{
					if (chk_status[i][j].way[k] == 3 && open_check(i, j, k, 3, maps,me) && open_check(i, j, (4 + k) % 8, 1, maps,me))
					{
						return{ j,i };
					}
				}
			}
		}
	}

	return {-1,-1};
}

ordered_pair check_open_two(status my_stat[][MAXXY], status enemy_stat[][MAXXY], int maps[][MAXXY], bool me)
{
	status(*chk_status)[MAXXY];
	if (me)
	{
		chk_status = my_stat;
	}
	else
	{
		chk_status = enemy_stat;
	}

	int i = 0;
	int j = 0;
	for (; i<MAXXY; ++i)
	{
		j = 0;
		for (; j<MAXXY; ++j)
		{
			if (chk_status[i][j].is_check == true)
			{
				int k = 0;
				for (; k<8; ++k)
				{
					if (chk_status[i][j].way[k] == 3 && open_check(i, j, k, 3, maps, me) && open_check(i, j, (4 + k) % 8, 1, maps, me))
					{
						return{ j,i };
					}
					else if (k < 4 && chk_status[i][j].way[k] == 2 && chk_status[i][j].way[(4 + k) % 8] == 2 && open_check(i, j, k, 3, maps, me) && open_check(i, j, (4 + k) % 8, 3, maps, me))
					{
						return{ j,i };
					}
				}
			}
		}
	}

	return{ -1,-1 };
}


ordered_pair eval_weight() {
	ordered_pair ret;
	ret = check_five(my_status,enemy_status,map,true);
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_five(my_status, enemy_status, map, false);
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_four_four(my_status, enemy_status, map, true);
	if (ret.x > -1)
	{
		
		return ret;
	}
	ret = check_open_four(my_status, enemy_status, map, true);
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_four_four(my_status,enemy_status,map,false);
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_four_three(my_status, enemy_status, map, true);
	if (ret.x > -1)
	{
		return ret;
	}
	
	ret = check_four_three(my_status, enemy_status, map, false);
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_open_four(my_status, enemy_status, map, false);
	if (ret.x > -1)
	{
		return ret;
	}
	ret = check_close_four(my_status, enemy_status, map, true);
	if (ret.x > -1)
	{
		
		return ret;
	}
	ret = check_three_three(my_status, enemy_status, map, true);
	if (ret.x > -1)
	{
		return ret;
	}
	
	ret = check_three_three(my_status, enemy_status, map, false);
	if (ret.x > -1)
	{
		return ret;
	}
	
	/*ret = check_close_three(my_status, enemy_status, map, true);
	if (ret.x > -1)
	{
		return ret;
	}*/
CString s;
	{
		int i,j,k;
		int max_count = -1;
		ordered_pair ret = { -1,-1 };
		
		for (i = 0; i < MAXXY; ++i)
		{
			for (j = 0; j < MAXXY; ++j)
			{
				if (enemy_status[i][j].is_check == true)
				{
					for (k = 0; k < 8; ++k)
					{
						
						if (k % 2 == 0 && enemy_status[i + way[k].y][j + way[k].x].is_check == true && open_check(i + way[k].y, j + way[k].x, k, 2, map, false) && open_check(i, j, (4 + k) % 8, 1, map, false))
						{
							int count = 0;
							if (enemy_status[i + way[k].y][j + way[k].x].way[k] == 2)
							{
								++count;
							}
							if (enemy_status[i][j].way[(k - 1 + 8) % 8] == 2 && open_check(i + way[(k - 1 + 8) % 8].y, j + way[(k - 1 + 8) % 8].x, (k - 2 + 8) % 8, 1, map, false))
							{
								++count;
							}
							if (enemy_status[i][j].way[(k + 1 + 8) % 8] == 2 && open_check(i + way[(k + 1 + 8) % 8].y, j + way[(k + 1 + 8) % 8].x, (k + 2 + 8) % 8, 1, map, false))
							{
								++count;
							}
							if (enemy_status[i][j].way[(k - 1 + 8) % 8] == 3 && open_check(i, j, (k - 1 + 8) % 8, 3, map, false) && open_check(i, j, (3 + k) % 8, 1, map, false) && open_check(i + way[(k - 1 + 8) % 8].y, j + way[(k - 1 + 8) % 8].x, (k - 2 + 8) % 8, 1, map, false))
							{
								++count;
							}
							if (enemy_status[i][j].way[(k + 1 + 8) % 8] == 3 && open_check(i, j, (k + 1 + 8) % 8, 3, map, false) && open_check(i, j, (5 + k) % 8, 1, map, false) && open_check(i + way[(k + 1 + 8) % 8].y, j + way[(k + 1 + 8) % 8].x, (k + 2 + 8) % 8, 1, map, false))
							{
								++count;
							}
							if (count > 2 && count > max_count)
							{
								max_count = count;
								ret = {j,i};
							}
						}
					}
				}
			}
		}
		if (ret.x > -1)
		{
			AfxMessageBox("1");
			return ret;
		}
	}
	
	int i = 0;
	int j = 0;
	int c;
	int d;
	int level = -1;
	ordered_pair tmpret = {-1,-1};

	for (c = 0; c < MAXXY; ++c)
	{
		for (d = 0; d < MAXXY; ++d)
		{
			if (my_status[c][d].is_check == true || enemy_status[c][d].is_check == true)
			{
				status my_stat[MAXXY][MAXXY];
				status enemy_stat[MAXXY][MAXXY];
				int map_one[MAXXY][MAXXY];

				memcpy(map_one, map, sizeof(map_one));
				memcpy(my_stat, my_status, sizeof(my_stat));
				map_one[c][d] = my_color;
				my_stat[c][d].is_check = false;
				update_status(d, c, my_stat, NULL, map_one, true);
				ret = check_four_four(my_stat, NULL, map_one, true);
				if (ret.x > -1)
				{
					ordered_pair ret5 = check_five(my_stat, NULL, map_one, true);
					if (ret.x != ret5.x && ret.y != ret5.y)
					{
						s.Format("I44 assume %d %d : %d %d",d,c,ret.x,ret.y);
						AfxMessageBox(s);
						return{ d,c };
					}
					ret5 = check_open_four(my_stat, NULL, map_one, true);
					if (ret.x != ret5.x && ret.y != ret5.y)
					{
						s.Format("I44 assume %d %d : %d %d", d, c, ret.x, ret.y);
						AfxMessageBox(s);
						return{ d,c };
					}
					else
					{
						if (level > 0)
						{
							level = 0;
							tmpret = {d,c};
						}
					}
				}
				
				memcpy(map_one, map, sizeof(map_one));
				memcpy(enemy_stat, enemy_status, sizeof(my_stat));
				map_one[c][d] = enemy_color;
				enemy_stat[c][d].is_check = false;
				update_status(d, c, NULL, enemy_stat, map_one, false);
				ret = check_four_four(NULL, enemy_stat, map_one, false);
				if (ret.x > -1)
				{
					s.Format("E44 assume %d %d : %d %d", d, c, ret.x, ret.y);
					AfxMessageBox(s);
					return{ d,c };
					/*ordered_pair ret5 = check_five(NULL, enemy_stat, map_one, false);
					if (ret.x != ret5.x && ret.y != ret5.y)
					{
						return{ d,c };
					}
					ret5 = check_open_four(NULL, enemy_stat, map_one, false);
					if (ret.x != ret5.x && ret.y != ret5.y)
					{
						return{ d,c };
					}
					else
					{
						if (level > 1)
						{
							level = 1;
							tmpret = {d,c};
						}
					}*/
				}

				memcpy(map_one, map, sizeof(map_one));
				memcpy(my_stat, my_status, sizeof(my_stat));
				map_one[c][d] = my_color;
				my_stat[c][d].is_check = false;
				update_status(d, c, my_stat, NULL, map_one, true);
				ret = check_four_three(my_stat, NULL, map_one, true);
				if (ret.x > -1)
				{
					ordered_pair ret5 = check_five(my_stat, NULL, map_one, true);
					if (ret.x != ret5.x && ret.y != ret5.y)
					{
						s.Format("I43 assume %d %d : %d %d", d, c, ret.x, ret.y);
						AfxMessageBox(s);
						return{ d,c };
					}
					ret5 = check_open_four(my_stat, NULL, map_one, true);
					if (ret.x != ret5.x && ret.y != ret5.y)
					{
						s.Format("I43 assume %d %d : %d %d", d, c, ret.x, ret.y);
						AfxMessageBox(s);
						return{ d,c };
					}
					else
					{
						if (level > 2)
						{
							level = 2;
							tmpret = { d,c };
						}
					}
				}

				memcpy(map_one, map, sizeof(map));
				memcpy(enemy_stat, enemy_status, sizeof(enemy_stat));
				map_one[c][d] = enemy_color;
				enemy_stat[c][d].is_check = false;
				update_status(d, c, NULL, enemy_stat, map_one, false);
				ret = check_four_three(NULL, enemy_stat, map_one, false);
				if (ret.x > -1)
				{
					ordered_pair ret5 = check_five(NULL, enemy_stat, map_one, false);
					if (ret.x != ret5.x && ret.y != ret5.y)
					{
						s.Format("E43 assume %d %d : %d %d", d, c, ret.x, ret.y);
						AfxMessageBox(s);
						return ret;
					}
					ret5 = check_open_four(NULL, enemy_stat, map_one, false);
					if (ret.x != ret5.x && ret.y != ret5.y)
					{
						s.Format("E43 assume %d %d : %d %d", d, c, ret.x, ret.y);
						AfxMessageBox(s);
						return ret;
					}

					else
					{
						if (level > 3)
						{
							level = 3;
							tmpret = ret;
						}
					}
				}

				memcpy(map_one, map, sizeof(map_one));
				memcpy(my_stat, my_status, sizeof(my_stat));
				map_one[c][d] = my_color;
				my_stat[c][d].is_check = false;
				update_status(d, c, my_stat, NULL, map_one, true);
				ret = check_three_three(my_stat, NULL, map_one, true);
				if (ret.x > -1)
				{
					ordered_pair ret5 = check_five(my_stat, NULL, map_one, true);
					if (ret.x != ret5.x && ret.y != ret5.y)
					{
						s.Format("I33 assume %d %d : %d %d", d, c, ret.x, ret.y);
						AfxMessageBox(s);
						return{ d,c };
					}
					ret5 = check_open_four(my_stat, NULL, map_one, true);
					if (ret.x != ret5.x && ret.y != ret5.y)
					{
						s.Format("I33 assume %d %d : %d %d", d, c, ret.x, ret.y);
						AfxMessageBox(s);
						return{ d,c };
					}
					else
					{
						if (level > 4)
						{
							level = 4;
							tmpret = { d,c };
						}
					}
				}

				/*memcpy(map_one, map, sizeof(map_one));
				memcpy(my_stat, my_status, sizeof(my_stat));
				map_one[c][d] = my_color;
				my_stat[c][d].is_check = false;
				update_status(d, c, my_stat, NULL, map_one, true);
				ret = check_five(my_stat, NULL, map_one, true);
				if (ret.x > -1)
				{
					if (level > 5)
					{
						level = 5;
						tmpret = { d,c };
					}
				}*/

				memcpy(map_one,map,sizeof(map));
				memcpy(enemy_stat, enemy_status, sizeof(enemy_stat));
				map_one[c][d] = enemy_color;
				enemy_stat[c][d].is_check = false;
				update_status(d, c, NULL, enemy_stat, map_one, false);
				ret = check_three_three(NULL, enemy_stat, map_one, false);
				if (ret.x > -1)
				{
					ordered_pair ret5 = check_five(NULL, enemy_stat, map_one, false);
					if (ret.x != ret5.x && ret.y != ret5.y)
					{
						s.Format("E33 assume %d %d : %d %d", d, c, ret.x, ret.y);
						AfxMessageBox(s);
						return ret;
					}
					ret5 = check_open_four(NULL, enemy_stat, map_one, false);
					if (ret.x != ret5.x && ret.y != ret5.y)
					{
						s.Format("E33 assume %d %d : %d %d", d, c, ret.x, ret.y);
						AfxMessageBox(s);
						return ret;
					}
					else
					{
						if (level > 5)
						{
							level = 5;
							tmpret = ret;
						}
					}
				}

			}
		}
	}

	if (level > -1)
	{
		return tmpret;
	}
	/*if (tmpret.x < 0)
	{
		int i,j;
		for (i = 0; i < MAXXY; ++i)
		{
			for (j = 0; j < MAXXY; ++j)
			{
				if (enemy_status[i][j].is_check == true)
				{
					int k;
					int count = 0;
					for (k = 0; k < 8; ++k)
					{
						if (enemy_status[i][j].way[k] = 2 && open_check(i,j,k,2,map,false) && open_check(i,j,(4 + k) % 8,1,map,false))
						{
							++count;
						}
						if (count > 2)
						{
							return {j,i};
						}
					}
				}
			}
		}
	}*/
	
	/*ret = check_open_two(my_status, enemy_status, map, false);
	if (ret.x > -1)
	{
		AfxMessageBox("sss");
		return ret;
	}*/

	i = 0;
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
					here += enemy_status[i][j].way[k] * 3;
					if (map[i + way[k].y][j + way[k].x] == EMPTY)
					{
						++here;
					}
				}
			}
			if (my_status[i][j].is_check == true)
			{
				for (k = 0; k < 8; ++k)
				{
					here += enemy_status[i][j].way[k] * 2;
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
		ordered_pair enemy = get_map();
		if (enemy.x > -1)
		{
			update_status(enemy.x, enemy.y,my_status,enemy_status,map, false);
		}
		ret = eval_weight();
	}

/*	
	if (CurTurn == 1)
	ret = { 5,3 };
	else if (CurTurn == 3)
	ret = { 10,3 };
	else if (CurTurn == 5)
	ret = { 5,5 };
	else if (CurTurn == 7)
	ret = { 4,6 };
	else if (CurTurn == 9)
	ret = { 6,6 };
	else if (CurTurn == 11)
	ret = { 7,6 };
	*/

	*NewX = ret.x;
	*NewY = ret.y;
	map[ret.y][ret.x] = my_color;
	my_status[ret.y][ret.x].is_check = false;
	enemy_status[ret.y][ret.x].is_check = false;
	update_status(ret.x, ret.y, my_status,enemy_status,map, true);
}
