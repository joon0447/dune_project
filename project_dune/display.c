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

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_object_info();
void display_system_message();
void display_commands();
void print_system_message(char text[]);

char system_message[8][100] = { "" };
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
			// �����
			if (map[1][i][j] == 'W') {
				printBgc(padd(map_pos, pos), 'W', COLOR_BLACK, COLOR_YELLOW);
			}

			// ����� �輳 �����̽� 
			else if (map[0][i][j] == 's') {
				printBgc(padd(map_pos, pos), spice, COLOR_WHITE, COLOR_ORANGE);
			}

			char back = backbuf[i][j];
			if (back == 'B') {
				if (i == MAP_HEIGHT - 3 && j == 1)
					printBgc(padd(map_pos, pos), 'B', COLOR_WHITE, COLOR_BLUE);
				else printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_BLUE, COLOR_BLUE);

				if (i >= MAP_HEIGHT - 17 && i < MAP_HEIGHT - 15 && j >= MAP_WIDTH - 3 && j < MAP_WIDTH - 1) { // AI ����
					if (i == MAP_HEIGHT - 17 && j == MAP_WIDTH - 3) printBgc(padd(map_pos, pos), 'B', COLOR_DEFAULT, COLOR_RED);
					else printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_RED, COLOR_RED);
				}
			}
			else if (back == 'P') {
				if (i == MAP_HEIGHT - 3 && j == 3) printBgc(padd(map_pos, pos), 'P', COLOR_BLACK, COLOR_DEFAULT);
				else printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT, COLOR_DEFAULT);

				if (i >= MAP_HEIGHT - 17 && i < MAP_HEIGHT - 15 && j >= MAP_WIDTH - 5 && j < MAP_WIDTH - 3) { // AI ���� ���� ����
					if (i == MAP_HEIGHT - 17 && j == MAP_WIDTH - 5) printBgc(padd(map_pos, pos), 'P', COLOR_BLACK, COLOR_DEFAULT);
					else printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT, COLOR_DEFAULT);
				}
			}
			else if (back == 'H') {
				if (i == MAP_HEIGHT - 4 && j == 1) { // �÷��̾� ���� �� �Ϻ�����
					printBgc(padd(map_pos, pos), 'H', COLOR_WHITE, COLOR_BLUE);
				}
				if (i == MAP_HEIGHT - 15 && j == MAP_WIDTH - 2) { // AI ���� �Ϻ�����
					printBgc(padd(map_pos, pos), 'H', COLOR_WHITE, COLOR_RED);
				}
			}
			else if (back == 'S') {
				if (i == MAP_HEIGHT - 6 && j == 1) { // �÷��̾� ���� �� �����̽�
					printBgc(padd(map_pos, pos), spice, COLOR_WHITE, COLOR_ORANGE);
				}
				if (i == MAP_HEIGHT - 13 && j == MAP_WIDTH - 2) { // AI ���� �����̽�
					printBgc(padd(map_pos, pos), spice, COLOR_WHITE, COLOR_ORANGE);
				}
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
	printc(padd(map_pos, prev), ch_prev, COLOR_DEFAULT);

	char ch_curr = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch_curr, COLOR_CURSOR);

}

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
		strcpy(system_message[i], system_message[i - 1]);
	}

	strcpy(system_message[0], text);

	// �ִ� 8��
	POSITION pos = { MAP_HEIGHT + 3, 1 };
	for (int i = 8; i > 0; i--) {
		print_info(padd(pos, (POSITION) { i, 1 }), system_message[8 - i], COLOR_WHITE);
	}
}

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
	for (int i = 0; i < 30; i++) {
		printc(padd(info_pos, (POSITION) { 1, i }), ' ', COLOR_WHITE);
	}
	print_info(padd(info_pos, (POSITION) { 1, 1 }), text, COLOR_WHITE);
}
