/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#define _CRT_SECURE_NO_WARNINGS
#include "display.h"
#include "io.h"


// ����� ������� �»��(topleft) ��ǥ
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

char system_message[8][100] = { "" };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_object_info();
void display_system_message();
void display_commands();
void object_cmd2(char text[], char text1[], char text2[], char text3[], char text4[]);


bool big_cursor = false;
bool prev_cursor_is_big = false;
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	display_object_info();
	display_system_message();
	display_commands();
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}


void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);
	
	char spice = '0' + 5;

	int num = rand() % 8 + 1;
	char rand_spice = '0' + num;
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			POSITION pos = { i,j };
			char layer_1 = map[1][i][j];
			// �����
			if (layer_1 == 'W') {
				printBgc(padd(map_pos, pos), 'W', COLOR_BLACK, COLOR_YELLOW);
			}

			

			char back = backbuf[i][j];
			if (back == 0) {
				printBgc(padd(map_pos, pos), 'P', COLOR_BLACK, COLOR_ORANGE);
			}
			// ����� �輳 �����̽� 
			if (back >= 49 && back <= 57) {
				printBgc(padd(map_pos, pos), back, COLOR_WHITE, COLOR_ORANGE);
			}
			else if (back == 'B') {
				if (i >= MAP_HEIGHT - 3 && i < MAP_HEIGHT - 1 && j >= 1 && j < 3) { // �÷��̾� ����
					if (i == MAP_HEIGHT - 3 && j == 1) printBgc(padd(map_pos, pos), 'B', COLOR_WHITE, COLOR_BLUE);
					else printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_BLUE, COLOR_BLUE);
				}
				if (i >= MAP_HEIGHT - 17 && i < MAP_HEIGHT - 15 && j >= MAP_WIDTH - 3 && j < MAP_WIDTH - 1) { // AI ����
					if (i == MAP_HEIGHT - 17 && j == MAP_WIDTH - 3) printBgc(padd(map_pos, pos), 'B', COLOR_DEFAULT, COLOR_RED);
					else printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_RED, COLOR_RED);
				}
			}
			else if (back == 'P') {
				printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT, COLOR_DEFAULT);
			}
			else if (back == 'p') {
				printBgc(padd(map_pos, pos), 'P', COLOR_BLACK, COLOR_DEFAULT);
			}
			else if (back == 'H') {
				printBgc(padd(map_pos, pos), 'H', COLOR_WHITE, COLOR_BLUE);
			}
			else if (back == 'h') {
				printBgc(padd(map_pos, pos), 'H', COLOR_WHITE, COLOR_RED);
			}
			else if (back == 'S') {
				printBgc(padd(map_pos, pos), spice, COLOR_WHITE, COLOR_ORANGE);
			}
			else if (back == 'x') {
				printBgc(padd(map_pos, pos), ' ', COLOR_BLACK, COLOR_BLACK);
			}
			else if (back == 'd') {
				printBgc(padd(map_pos, pos), 'D', COLOR_WHITE, COLOR_GREEN);
			}
			else if (back == 'D') {
				printBgc(padd(map_pos, pos), ' ', COLOR_GREEN, COLOR_GREEN);
			}
			else if (back == 'g') {
				printBgc(padd(map_pos, pos), 'G', COLOR_WHITE, COLOR_OK);
			}
			else if (back == 'G') {
				printBgc(padd(map_pos, pos), ' ', COLOR_OK, COLOR_OK);
			}
			else if (back == 'a') {
				printBgc(padd(map_pos, pos), 'B', COLOR_WHITE, COLOR_PURPLE);
			}
			else if (back == 'A') {
				printBgc(padd(map_pos, pos), ' ', COLOR_WHITE, COLOR_PURPLE);
			}
			else if (back == 'r') {
				printBgc(padd(map_pos, pos), 'S', COLOR_WHITE, COLOR_P);
			}
			else if (back == 'z') {
				printBgc(padd(map_pos, pos), ' ', COLOR_WHITE, COLOR_P);
			}
			else if (back == 'o') {
				printBgc(padd(map_pos, pos), 'S', COLOR_WHITE, COLOR_LRED);
			}
		}
	}

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = { i, j };

				if (i == MAP_HEIGHT - 8 && j == MAP_WIDTH - 10) { // ����
					printBgc(padd(map_pos, pos), 'R', COLOR_WHITE, COLOR_GRAY);
					backbuf[i][j] = 'R';
				}
				else if (i == MAP_HEIGHT - 10 && j == MAP_WIDTH - 42) { // ����
					printBgc(padd(map_pos, pos), 'R', COLOR_WHITE, COLOR_GRAY);
					backbuf[i][j] = 'R';
				}
				else if (i == MAP_HEIGHT - 5 && j == MAP_WIDTH - 25) { // ����
					printBgc(padd(map_pos, pos), 'R', COLOR_WHITE, COLOR_GRAY);
					backbuf[i][j] = 'R';
				}
				else if (i >= MAP_HEIGHT - 12 && i< MAP_HEIGHT- 10 && j >= MAP_WIDTH - 35 && j < MAP_WIDTH - 33) { // ����
					if(i==MAP_HEIGHT - 12 && j == MAP_WIDTH - 35) printBgc(padd(map_pos, pos), 'R', COLOR_WHITE, COLOR_GRAY);
					else printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_GRAY, COLOR_GRAY);
					backbuf[i][j] = 'R';
				}
				else if (i >= MAP_HEIGHT - 5 && i < MAP_HEIGHT - 3 && j >= MAP_WIDTH - 12 && j < MAP_WIDTH - 10) { // ����
					if(i==MAP_HEIGHT -5 && j == MAP_WIDTH -12) printBgc(padd(map_pos, pos), 'R', COLOR_WHITE, COLOR_GRAY);
					else printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_GRAY, COLOR_GRAY);
					backbuf[i][j] = 'R';
				}
				else {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
				}
				
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}


// ����â
void display_object_info() {
	int info_width = 50;   // ����â�� �ʺ�
	POSITION info_pos = { 1, MAP_WIDTH + 2 }; // ����â�� ���� ��ġ (1, MAP_WIDTH + 2)

	printf("\033[?25l"); // �׵θ� Ŀ�� �����
	
	for (int i = 0; i < MAP_HEIGHT; i++) {
		printc(padd(info_pos, (POSITION) { i, 0 }), '|', COLOR_WHITE); // ���� �׵θ�
		printc(padd(info_pos, (POSITION) { i, info_width - 1 }), '|', COLOR_WHITE); // ������ �׵θ�
	}
	for (int j = 0; j < info_width; j++) {
		printc(padd(info_pos, (POSITION) { 0, j }), '-', COLOR_WHITE); // ��� �׵θ�
		printc(padd(info_pos, (POSITION) { MAP_HEIGHT - 1, j }), '-', COLOR_WHITE); // �ϴ� �׵θ�
	}
}





// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;
	char ch_prev = backbuf[prev.row][prev.column];
	char ch_curr = frontbuf[curr.row][curr.column];
	POSITION x = { 1,0 };
	POSITION y = { 0,1 };
	POSITION xy = { 1,1 };
	POSITION prev_a = padd(prev, x);
	POSITION prev_b = padd(prev, y);
	POSITION prev_c = padd(prev, xy);

	POSITION curr_a = padd(curr, x);
	POSITION curr_b = padd(curr, y);
	POSITION curr_c = padd(curr, xy);

	if (!big_cursor) {
		if (prev_cursor_is_big) {
			printc(padd(map_pos, prev_a), ch_prev, COLOR_DEFAULT);
			printc(padd(map_pos, prev_b), ch_prev, COLOR_DEFAULT);
			printc(padd(map_pos, prev_c), ch_prev, COLOR_DEFAULT);
			printc(padd(map_pos, prev), ch_prev, COLOR_DEFAULT);
			printc(padd(map_pos, curr), ch_curr, COLOR_DEFAULT);
			printc(padd(map_pos, curr_a), ch_curr, COLOR_DEFAULT);
			printc(padd(map_pos, curr_b), ch_curr, COLOR_DEFAULT);
			printc(padd(map_pos, curr_c), ch_curr, COLOR_DEFAULT);
			prev_cursor_is_big = false;
		}
		printc(padd(map_pos, prev), ch_prev, COLOR_DEFAULT);
		printc(padd(map_pos, curr), ch_curr, COLOR_CURSOR);
	}
	else{
		prev_cursor_is_big = true;
		printc(padd(map_pos, prev_a), ch_prev, COLOR_DEFAULT);
		printc(padd(map_pos, prev_b), ch_prev, COLOR_DEFAULT);
		printc(padd(map_pos, prev_c), ch_prev, COLOR_DEFAULT);
		printc(padd(map_pos, prev), ch_prev, COLOR_DEFAULT);
		printc(padd(map_pos, curr), ch_curr, COLOR_CURSOR);
		printc(padd(map_pos, curr_a), ch_curr, COLOR_CURSOR);
		printc(padd(map_pos, curr_b), ch_curr, COLOR_CURSOR);
		printc(padd(map_pos, curr_c), ch_curr, COLOR_CURSOR);
	}
}

// �ý��� �޽��� â
void display_system_message() {
	int height = 10;
	POSITION pos = { MAP_HEIGHT + 2, 1 };
	printf("\033[?25l"); // �׵θ� Ŀ�� �����
	for (int i = 0; i < height; i++) {
		printc(padd(pos, (POSITION) { i, 0 }), '|', COLOR_WHITE); // ���� �׵θ�
		printc(padd(pos, (POSITION) { i, MAP_WIDTH - 2 }), '|', COLOR_WHITE); // ������ �׵θ�
	}

	for (int i = 0; i < MAP_WIDTH-1; i++) {
		printc(padd(pos, (POSITION) { 0, i }), '-', COLOR_WHITE); // ��� �׵θ�
		printc(padd(pos, (POSITION) { height - 1, i }), '-', COLOR_WHITE); // �ϴ� �׵θ�
	}
}

void print_system_message(char text[]) {
	char tmp[] = { system_message[0] };
	
	for (int i = 7; i > 0; i--) {
		strcpy(system_message[i], system_message[i-1]);
	}

	strcpy(system_message[0], text);
	
	// �ִ� 8��
	POSITION pos = { MAP_HEIGHT + 3, 1 };
	for (int i = 7; i > 0; i--) {
		print_info(padd(pos, (POSITION) { i, 1 }), system_message[7-i], COLOR_WHITE);
	}
}

// ���â
void display_commands() {
	int info_width = 50;   // ����â�� �ʺ�
	int height = 10;
	POSITION info_pos = { MAP_HEIGHT+2, MAP_WIDTH + 2 }; // ����â�� ���� ��ġ (1, MAP_WIDTH + 2)
	printf("\033[?25l"); // �׵θ� Ŀ�� �����
	for (int i = 0; i < height; i++) {
		printc(padd(info_pos, (POSITION) { i, 0 }), '|', COLOR_WHITE); // ���� �׵θ�
		printc(padd(info_pos, (POSITION) { i, info_width - 1 }), '|', COLOR_WHITE); // ������ �׵θ�
	}
	for (int j = 0; j < info_width; j++) {
		printc(padd(info_pos, (POSITION) { 0, j }), '-', COLOR_WHITE); // ��� �׵θ�
		printc(padd(info_pos, (POSITION) { height - 1, j }), '-', COLOR_WHITE); // �ϴ� �׵θ�
	}
}

void object_info(char text[]) {
	POSITION info_pos = { 1, MAP_WIDTH + 2 };
	for (int i = 0; i < 30; i++) {
		printc(padd(info_pos, (POSITION) { 1, i }), ' ', COLOR_WHITE);
	}

	print_info(padd(info_pos, (POSITION) { 1, 1 }), text, COLOR_WHITE);
}

void object_cmd	(char text[]) {
	POSITION info_pos = { 20, MAP_WIDTH + 2 };
	for (int i = 0; i < 50; i++) {
		printc(padd(info_pos, (POSITION) { 1, i }), ' ', COLOR_WHITE);
		printc(padd(info_pos, (POSITION) { 2, i }), ' ', COLOR_WHITE);
		printc(padd(info_pos, (POSITION) { 3, i }), ' ', COLOR_WHITE);
		printc(padd(info_pos, (POSITION) { 4, i }), ' ', COLOR_WHITE);
		printc(padd(info_pos, (POSITION) { 5, i }), ' ', COLOR_WHITE);
		printc(padd(info_pos, (POSITION) { 6, i }), ' ', COLOR_WHITE);
	}
	print_info(padd(info_pos, (POSITION) { 1, 1 }), text, COLOR_WHITE);
}

void object_cmd2(char text[], char text1[], char text2[], char text3[], char text4[], char text5[]) {
	POSITION start_pos = { 20, MAP_WIDTH + 2 };
	for (int i = 0; i < 50; i++) {
		printc(padd(start_pos, (POSITION) { 1, i }), ' ', COLOR_WHITE);
		printc(padd(start_pos, (POSITION) { 2, i }), ' ', COLOR_WHITE);
		printc(padd(start_pos, (POSITION) { 3, i }), ' ', COLOR_WHITE);
		printc(padd(start_pos, (POSITION) { 4, i }), ' ', COLOR_WHITE);
		printc(padd(start_pos, (POSITION) { 5, i }), ' ', COLOR_WHITE);
		printc(padd(start_pos, (POSITION) { 6, i }), ' ', COLOR_WHITE);
	}
	print_info(padd(start_pos, (POSITION) { 1, 1 }), text, COLOR_WHITE);	
	print_info(padd(start_pos, (POSITION) { 2, 1 }), text1, COLOR_WHITE);
	print_info(padd(start_pos, (POSITION) { 3, 1 }), text2, COLOR_WHITE);
	print_info(padd(start_pos, (POSITION) { 4, 1 }), text3, COLOR_WHITE);
	print_info(padd(start_pos, (POSITION) { 5, 1 }), text4, COLOR_WHITE);
	print_info(padd(start_pos, (POSITION) { 6, 1 }), text5, COLOR_WHITE);
}