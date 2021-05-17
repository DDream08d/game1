/* Headers ----------------------------------------------------------- */
#include <stdio.h>
#include <windows.h>	// for consol window
#include <conio.h>		// for recognizing kbd value.
#include <time.h>		
#include <stdlib.h>

/* Definitions ------------------------------------------------------- */
#define WIDTH		15
#define HEIGHT		13
#define SPACE		2

#define TRUE		1
#define FALSE		0

#define SYSTEM_X	5			//for system msg output
#define SYSTEM_Y	HEIGHT+5

#define	DIE			-1
#define EMPTY		0	// * 공간 : " ", color : 
#define ME			1	// * 나 : ●, color : 14, 
#define ENEMY		2	// * 적 : ○, color : 15, 
#define SCORING		3	//
#define WALL		9	// * 벽: ▣, color : 10,

#define MAX_ENEMY	3

/* Structures -------------------------------------------------------- */
enum STAT
{
	DEAD,		//0
	ALIVE=1,	//1
	EATING		//2
};

struct location
{
	int x;	int y;	enum STAT status;
}my, my_pre, enemy;

static int is_Pushed = 0;
static int is_Pushed_pre = 0;

static int gaming_field[WIDTH][HEIGHT];

static int no_of_enemys = 0;
static int length = 0;

static int tail[10][2];// 초기화를 일단 시키기 말자= { {0,0}, {0,0}, {0,0} };

/* Function List ----------------------------------------------------- */
void init_system();
void textcolor(int color_number);
void print_error(int err);
void print_system(int msg);
void print_playtime();
void gotoxy_system(int x, int y);
void gotoxy(int x, int y);
char get_Key();
void move_my_location(char key);
void init_my_position();
void init_timer();
void enemy_spotted();
void init_field();
void draw_me(char key);
void draw_wall();
void draw_scoring();
void draw_tail();
void init_loading();

/* Main function ----------------------------------------------------- */
int main()
{
	init_system();
	
	init_loading();
	//TODO: 빈 화면에 키 설명 -> press any key -> 본격 시작.

	init_field();

	init_my_position();
	init_timer();

	char key = NULL;
	my.status = ALIVE;
	
	while (1)		//main loop, 
	{
		is_Pushed_pre = is_Pushed;
		is_Pushed = _kbhit();
		if (is_Pushed == TRUE)	//it will work when the key is toggled
		{
			if (is_Pushed_pre == FALSE)
			{
				key = get_Key();

				if (no_of_enemys <= MAX_ENEMY)
				{
					enemy_spotted();	
					no_of_enemys++;
				}
				else
				{
					// do nothing
				}

				move_my_location(key);	//move my location and print the status
										//my_location, my_pre_location is changing here 
				{//for checking location
					gotoxy_system(3, HEIGHT + 6);
					printf("my.x/my.y : %d / %d", my.x, my.y);
				}
				
				
			}
			else
			{
				key = NULL;
			}
		}
		else
		{
			key = NULL;
		}

		gotoxy_system(3, HEIGHT + 2);		// tester : print input keys
		printf("input key : %d", key);

		gotoxy_system(3, HEIGHT + 3);		// no. of enemys
		printf("enemys : %d", no_of_enemys);
		
		gotoxy_system(3, HEIGHT + 4);		// length of me
		printf("length : %d", length);

		my_pre = my;
		print_playtime();

		if (my.status == DEAD)
		{
			//GAME OVER//
			Sleep(1000);
			print_error(00);
			Sleep(2500);

			break;
		}
	}

}


/* Error msg ---------------------------------------------------------
 *
 * Code 01
 *   - definition : Location out of ragne
 *   - caller : gotoxy_system, gotoxy
 * ------------------------------------------------------------------- */
void print_error(int err)
{
	gotoxy_system(SYSTEM_X, SYSTEM_Y);
	//printf("! Error occured : ");
	switch (err)
	{
	case 00:
		textcolor(12);
		gotoxy_system(SYSTEM_X - 1, SYSTEM_Y + 1);
		printf(" GAME OVER!!! ");
		textcolor(15);
		break;
	case 01:
		textcolor(11);
		printf(" YOU DIE! ");
		textcolor(15);
		break;
	}	//TODO: error code to be re-organazed with "tydef enum"

}

/* System msg ---------------------------------------------------------
 *
 * Code 0
 *   - definition : Location out of ragne
 *   - caller : gotoxy_system, gotoxy
 * ------------------------------------------------------------------- */
void print_system(int msg)
{
	gotoxy_system(SYSTEM_X, SYSTEM_Y);
	switch (msg)
	{
	case 00:
		printf("                                   ");
		break;
	case 01:
		//printf("[ Code01 ] Location out of range");
		break;
	}
}
/* Functions --------------------------------------------------------- */

void init_system()
{
	system("mode con cols=37 lines=26");
}

void init_my_position()
{
	my.x = WIDTH/2;
	my.y = HEIGHT/2;
	my.status = ALIVE;
	gotoxy(my.x, my.y);
	draw_me('w');
	gaming_field[my.x][my.y] = ME;
}

void init_timer()
{
	// do nothing

//	int start1;
//	clock_t start2;
//	start2 = clock();
//	printf("time(NULL) : %d \n clock() : %d", start1, start2);
}

void textcolor(int color_number)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_number);
}

void gotoxy_system(int x, int y)
{
	COORD pos = { SPACE * x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	if ((x < 0) || (y < 0))
	{
		print_error(01);
	}
}

void gotoxy(int x, int y)
{
	COORD pos = { SPACE * x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	if ((x < 1) || (y < 1) || (x > WIDTH - 1) || (y > HEIGHT - 1))	// case : crashed to the wall
	{
		print_error(01);	// if it is crashed to the wall, dead
		my = my_pre;		// gotoxy 에서 my의 위치가 변경되는 경우 my_pre가 저장이 됨.
		gotoxy(my.x, my.y);
		my.status = DEAD;

	}
}

char get_Key()
{
	char key;
	key = _getch();
	return key;
}

void move_my_location(char key)
{
	my.status = ALIVE;
	switch (key)	// input keys(a, s, d, w) -> change location -> delete pre_location
	{				// is enemy? true -> scoring status  //  false -> move location
	case 'a':
		gotoxy(my.x, my.y);
		gaming_field[my.x][my.y] = EMPTY;

		if (gaming_field[my.x-1][my.y] == ENEMY)
		{
			my.status=EATING;
		}

		printf("  ");
		my.x--;
		break;
	case 's':
		gotoxy(my.x, my.y);
		gaming_field[my.x][my.y] = EMPTY;

		if (gaming_field[my.x][my.y+1] == ENEMY)
		{
			my.status = EATING;
		}

		printf("  ");
		my.y++;
		break;
	case 'd':
		gotoxy(my.x, my.y);
		gaming_field[my.x][my.y] = EMPTY;

		if (gaming_field[my.x+1][my.y] == ENEMY)
		{
			my.status = EATING;
		}

		printf("  ");
		my.x++;
		break;
	case 'w':
		gotoxy(my.x, my.y);
		gaming_field[my.x][my.y] = EMPTY;

		if (gaming_field[my.x][my.y-1] == ENEMY)
		{
			my.status = EATING;
		}

		printf("  ");
		my.y--;
		break;
	}

	gotoxy(my.x, my.y);
	if (my.status == ALIVE)	
	{
		draw_me(key);
		draw_tail();
		gaming_field[my.x][my.y] = ME;
	}
	else if (my.status == EATING)
	{
		draw_scoring();
		gaming_field[my.x][my.y] = SCORING;
		no_of_enemys--;
		length++;
	}
	else if (my.status == DEAD)
	{
		printf("†");
		gaming_field[my.x][my.y] = DIE;
	}


}

void enemy_spotted()
{
	enemy.x = rand() % (WIDTH - 2) + 1;
	enemy.y = rand() % (HEIGHT - 2) + 1;

	gotoxy(enemy.x, enemy.y);
	printf("○");	
	gaming_field[enemy.x][enemy.y] = ENEMY;

}

void print_playtime()
{
	int time = clock() / 1000;
	if (my.status == ALIVE)
	{
		gotoxy_system(WIDTH - 6, HEIGHT + 1);
		printf("play time : %d", time);
	}
	else
	{
		// do nothing
	}
}
void init_field()
{
	for (int j = 0; j < HEIGHT+1; j++)
	{
		for (int i = 0; i < WIDTH+1; i++)
		{
			if ((i == 0) || (j == 0) || (i == WIDTH) || (j == HEIGHT))
			{
				gaming_field[j][i] = WALL;	//9 
				gotoxy_system(i, j);
				draw_wall();
			}
			else
			{
				gotoxy_system(i, j);
				printf("  ");
				gaming_field[j][i] = EMPTY;	//0
			}
		}
	}
}
void draw_me(char key)
{
	textcolor(14);
	switch (key)	
	{				
	case 'a':
		printf("◀");
		break;
	case 's':
		printf("▼");
		break;
	case 'd':
		printf("▶");
		break;
	case 'w':
		printf("▲");
		break;
	}
	textcolor(15);
}

void draw_wall()
{
	textcolor(10);
	printf("▣");
	textcolor(15);
	Sleep(1);
}

void draw_scoring()
{
	textcolor(14);
	printf("★");
	textcolor(15);
}

void draw_tail()	//TODO:scoring 순간에도 꼬리가 나와야 함., TODO: 무슨 문제때문인지 꼬리가 나왔다가 사라짐.
{
	//is_Pushed_pre = is_Pushed;
	//is_Pushed = _kbhit();
	//if (is_Pushed == TRUE)	//it will work when the key is toggled
	//{
	//	if (is_Pushed_pre == FALSE)
	//	{
			if (length > 0)
			{
				for (int i = length; i >= 0; i--)
				{ // for the tail	
					tail[length][0] = my_pre.x;
					tail[length][1] = my_pre.y;
					for (int j = 0; j <= i; j++)
					{
						tail[j][0] = tail[j + 1][0];
						tail[j][1] = tail[j + 1][1];

						tail[length][0] = my_pre.x;
						tail[length][1] = my_pre.y;

						gotoxy_system(3, HEIGHT + 7);
						printf("tail[0][0]/tail[0][1] : %d / %d", tail[0][0], tail[0][1]);
						gotoxy_system(3, HEIGHT + 8);
						printf("tail[1][0]/tail[1][1] : %d / %d", tail[1][0], tail[1][1]);
						gotoxy_system(3, HEIGHT + 9);
						printf("tail[2][0]/tail[2][1] : %d / %d", tail[2][0], tail[2][1]);
						gotoxy_system(tail[j][0], tail[j][1]);
						printf("□");
						Sleep(500);
					}
					gotoxy_system(tail[0][0], tail[0][1]);
					printf("  ");
				}
			}
			else if (length == 0)
			{
				tail[1][0] = my_pre.x;
				tail[1][1] = my_pre.y;
			}
	//	}
	//}
}

void init_loading()
{
	for (int j = 0; j < HEIGHT + 1; j++)
	{
		for (int i = 0; i < WIDTH + 1; i++)
		{
			if ((i == 0) || (j == 0) || (i == WIDTH) || (j == HEIGHT))
			{
				gotoxy_system(i, j);
				printf("■");
				Sleep(50);
			}
			else
			{
				//do nothing
			}
		}
	}

	gotoxy_system(WIDTH / 3, HEIGHT / 2);
	printf("Press Any Key");

	while (1)
	{
		is_Pushed_pre = is_Pushed;
		is_Pushed = _kbhit();

		if (is_Pushed == TRUE)
		{
			if (is_Pushed_pre == FALSE)
			{
				break;
			}
			else
			{
				//do nothing
			}
		}
		else
		{
			//do nothing
		}
	}

	is_Pushed_pre = 0;
	is_Pushed = 0;

}
