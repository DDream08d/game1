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
void enemy_spotted();
void init_field();
void draw_me();
void draw_wall();
void draw_scoring();

/* Main function ----------------------------------------------------- */
int main()
{
	init_system();
	//TODO: 빈 화면에 키 설명 -> press any key -> 본격 시작.

	init_field();

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

				// field에서 no. of enemy를 체크하는 방법이 있음.
				// enemy_sppotted가 실행 될 때 마다 cnt 하는 방법도 있음.(단, field 값이 변경된다면, 매번 반영 되어야 함.)
				if (no_of_enemys <= MAX_ENEMY)
				{
					enemy_spotted();	//TODO: enemy가 없다면. enemy spotted실행되도록 하자.
					no_of_enemys++;
				}
				else
				{
					// do nothing
				}

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

		gotoxy_system(3, HEIGHT + 3);		// 입력 중인 키값 출력
		printf("enemys : %d", no_of_enemys);

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
	draw_me();
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
	if ((x < 1) || (y < 1) || (x > WIDTH - 1) || (y > HEIGHT - 1))	// 벽에 충돌 하는 경우
	{
		print_error(01);	// 벽에 충돌 msg : 쥬금
		my = my_pre;
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
	switch (key)	// 입력 키값에 따라 전에 기록된 값을 지우고 좌표값 변환.
	{				// is enemy? true -> false -> 이동
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
	if (my.status == ALIVE)	//나중에 print status로 묶어 버리자.
	{
		draw_me();
		gaming_field[my.x][my.y] = ME;
	}
	else if (my.status == EATING)
	{
		draw_scoring();
		gaming_field[my.x][my.y] = SCORING;
		no_of_enemys--;
	}
	else if (my.status == DEAD)
	{
		printf("†");
		gaming_field[my.x][my.y] = DIE;
	}
}	// game 의 한 togle은 버튼 누르는 걸 기준으로. 

void enemy_spotted()
{
	enemy.x = rand() % (WIDTH - 2) + 1;
	enemy.y = rand() % (HEIGHT - 2) + 1;

	gotoxy(enemy.x, enemy.y);
	printf("○");	// 각 좌표들을 배열로 선언한 다음에, is occupied 혹은 empty 0, 나 1, 적 2, 벽 9 로 지정해야 겠다.
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
				gaming_field[j][i] = EMPTY;	//0
			}
		}
	}
}
void draw_me()
{
	textcolor(14);
	printf("⊙");
	textcolor(15);
}

void draw_wall()
{
	textcolor(10);
	printf("▣");
	textcolor(15);
}

void draw_scoring()
{
	textcolor(9);
	printf("●");
	textcolor(15);
}