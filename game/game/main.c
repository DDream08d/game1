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

/* Structures -------------------------------------------------------- */
enum STAT
{
	DEAD,	//0
	ALIVE,	//1
	EATING	//2
};

struct location
{
	int x;	int y;	enum STAT status;
}my, my_pre, enemy;

static int is_Pushed = 0;
static int is_Pushed_pre = 0;

/* Function List ----------------------------------------------------- */
void init_system();
void textcolor(int color_number);
void print_error(int err);
void print_system(int msg);
void print_playtime();
void gotoxy_system(int x, int y);
void gotoxy(int x, int y);
void draw_field();
char get_Key();
void move_my_location(char key);
void init_my_position();
void init_timer();
void eney_spotted();


/* Main function ----------------------------------------------------- */
int main()
{
	init_system();

	draw_field();

	init_my_position();
	init_timer();


	char key = NULL;
	my.status = ALIVE;

	while (1)
	{
		is_Pushed_pre = is_Pushed;
		is_Pushed = _kbhit();
		if (is_Pushed == TRUE)
		{
			if (is_Pushed_pre == FALSE)
			{
				key = get_Key();
				eney_spotted();
				move_my_location(key); //-> move my location 으로 이동. my location 의 status가 alive 인경우 ㅇ dead -> ※, † 1초 간격으로 그 다음에 you die
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

		gotoxy_system(3, HEIGHT + 2);		// 입력 중인 키값 출력
		printf("input key : %d", key);

		my_pre = my;
		print_playtime();

		if (my.status == DEAD)
		{
			//GAME OVER// 나중에 dot로 찍어서 글씨 만들어야 함.
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
	my.x = 1;
	my.y = 1;
	my.status = ALIVE;

	gotoxy(1, 1);
	textcolor(14);
	printf("●");
	textcolor(15);
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

void gotoxy_system(int x_1, int y)
{
	int x = x_1 + 1;
	COORD pos = { SPACE * x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	if ((x < 0) || (y < 0))
	{
		print_error(01);
	}
}

void gotoxy(int x_1, int y)
{
	int x = x_1 + 1;
	COORD pos = { SPACE * x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	if ((x < 1) || (y < 1) || (x > WIDTH - 1) || (y > HEIGHT - 1))	// 벽에 충돌 하는 경우
	{
		print_error(01);	// 벽에 충돌 msg : 쥬금
		my = my_pre;
		gotoxy(my.x, my.y);
		my.status = DEAD;
	}
}

void draw_field()	// field range -> 1 ~ HEIGHT/WIDTH
{
	int i, j;
	textcolor(10);
	for (i = 0; i < WIDTH + 1; i++)
	{
		for (j = 0; j < HEIGHT + 1; j++)
		{
			gotoxy_system(i, j);
			printf("▣");
		}
	}

	for (i = 1; i < WIDTH; i++)
	{
		for (j = 1; j < HEIGHT; j++)
		{
			gotoxy_system(i, j);
			printf("  ");
		}
	}
	textcolor(15);
	printf("\n");
}

char get_Key()
{
	char key;
	key = _getch();
	return key;
}

void move_my_location(char key)
{
	switch (key)	// 입력 키값에 따라 전에 기록된 값을 지우고 좌표값 변환.
	{
	case 'a':
		gotoxy(my.x, my.y);
		printf("  ");
		my.x--;
		break;
	case 's':
		gotoxy(my.x, my.y);
		printf("  ");
		my.y++;
		break;
	case 'd':
		gotoxy(my.x, my.y);
		printf("  ");
		my.x++;
		break;
	case 'w':
		gotoxy(my.x, my.y);
		printf("  ");
		my.y--;
		break;
	}

	gotoxy(my.x, my.y);
	if (my.status == ALIVE)	//나중에 print status로 묶어 버리자.
	{
		textcolor(14);
		printf("●");
		textcolor(15);
	}
	else if (my.status == EATING)
	{
		printf("☆");
	}
	else if (my.status == DEAD)
	{
		printf("†");
	}
}	// game 의 한 togle은 버튼 누르는 걸 기준으로. 

void eney_spotted()
{
	enemy.x = rand() % (WIDTH - 2) + 1;
	enemy.y = rand() % (HEIGHT - 2) + 1;

	gotoxy(enemy.x, enemy.y);
	printf("○");	// 각 좌표들을 배열로 선언한 다음에, is occupied 혹은 empty 0, 나 1, 적 2, 벽 9 로 지정해야 겠다.

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