/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

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


void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	// display_system_message()
	// display_object_info()
	// display_commands()
	// ...
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
	
	// �����̽� ���差 ������ ���� ���� ���� ( 1 ~ 9 ����)
	srand((unsigned int)time(NULL));
	int rand_num = (rand() % 9) + 1;
	char spice = '0' + rand_num;


	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = { i, j };
				if (i >= MAP_HEIGHT - 3 && i < MAP_HEIGHT - 1 && j >= 1 && j < 3) { // �÷��̾� ����
					if (i == MAP_HEIGHT-3 && j == 1) printBgc(padd(map_pos, pos), 'B', COLOR_WHITE, COLOR_BLUE);
					else printBgc(padd(map_pos, pos), backbuf[i][j] , COLOR_BLUE, COLOR_BLUE);
					//if (i == MAP_HEIGHT - 3 && j == 1) map[0][i][j] = 'B'; // 'B'�� ������ �ǹ�
					//else map[0][i][j] = backbuf[i][j];
				}
				else if (i >= MAP_HEIGHT - 3 && i < MAP_HEIGHT - 1 && j >= 3 && j < 5) { // �÷��̾� ���� ���� ����
					if (i == MAP_HEIGHT - 3 && j == 3) printBgc(padd(map_pos, pos), 'P', COLOR_BLACK, COLOR_DEFAULT);
					else printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT, COLOR_DEFAULT);
				}
				else if (i == MAP_HEIGHT - 4 && j ==  1) { // �÷��̾� ���� �� �Ϻ�����
					printBgc(padd(map_pos, pos), 'H', COLOR_WHITE, COLOR_BLUE);
				}
				else if (i == MAP_HEIGHT - 6 && j == 1) { // �÷��̾� ���� �� �����̽�
					printBgc(padd(map_pos, pos), spice, COLOR_WHITE, COLOR_ORANGE);
				}
				else if (i == MAP_HEIGHT - 16 && j == 4) { // �÷��̾� ���� �� �����
					printBgc(padd(map_pos, pos), 'W', COLOR_BLACK, COLOR_YELLOW);
				}
				else if (i >= MAP_HEIGHT - 17 && i < MAP_HEIGHT - 15 && j >= MAP_WIDTH - 3 && j < MAP_WIDTH - 1) { // AI ����
					if(i==MAP_HEIGHT - 17 && j == MAP_WIDTH - 3) printBgc(padd(map_pos, pos), 'B', COLOR_DEFAULT, COLOR_RED);
					else printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_RED, COLOR_RED);
				}
				else if (i >= MAP_HEIGHT - 17 && i < MAP_HEIGHT - 15 && j >= MAP_WIDTH - 5 && j < MAP_WIDTH - 3) { // AI ���� ���� ����
					if(i== MAP_HEIGHT - 17 && j == MAP_WIDTH -5) printBgc(padd(map_pos, pos), 'P', COLOR_BLACK, COLOR_DEFAULT);
					else printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT, COLOR_DEFAULT);
				}
				else if (i == MAP_HEIGHT - 15 &&  j == MAP_WIDTH-2) { // AI ���� �Ϻ�����
					printBgc(padd(map_pos, pos), 'H', COLOR_WHITE, COLOR_RED);
				}
				else if (i == MAP_HEIGHT - 13 && j == MAP_WIDTH - 2) { // AI ���� �����̽�
					printBgc(padd(map_pos, pos), spice, COLOR_WHITE, COLOR_ORANGE);
				}
				else if (i == MAP_HEIGHT - 4 && j == MAP_WIDTH - 7) { // AI ���� �����
					printBgc(padd(map_pos, pos), 'W', COLOR_BLACK, COLOR_YELLOW);
				}
				else if (i == MAP_HEIGHT - 8 && j == MAP_WIDTH - 10) { // ����
					printBgc(padd(map_pos, pos), 'R', COLOR_WHITE, COLOR_GRAY);
				}
				else if (i == MAP_HEIGHT - 10 && j == MAP_WIDTH - 42) { // ����
					printBgc(padd(map_pos, pos), 'R', COLOR_WHITE, COLOR_GRAY);
				}
				else if (i == MAP_HEIGHT - 5 && j == MAP_WIDTH - 25) { // ����
					printBgc(padd(map_pos, pos), 'R', COLOR_WHITE, COLOR_GRAY);
				}
				else if (i >= MAP_HEIGHT - 12 && i< MAP_HEIGHT- 10 && j >= MAP_WIDTH - 35 && j < MAP_WIDTH - 33) { // ����
					if(i==MAP_HEIGHT - 12 && j == MAP_WIDTH - 35) printBgc(padd(map_pos, pos), 'R', COLOR_WHITE, COLOR_GRAY);
					else printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_GRAY, COLOR_GRAY);
				}
				else if (i >= MAP_HEIGHT - 5 && i < MAP_HEIGHT - 3 && j >= MAP_WIDTH - 12 && j < MAP_WIDTH - 10) { // ����
					if(i==MAP_HEIGHT -5 && j == MAP_WIDTH -12) printBgc(padd(map_pos, pos), 'R', COLOR_WHITE, COLOR_GRAY);
					else printBgc(padd(map_pos, pos), backbuf[i][j], COLOR_GRAY, COLOR_GRAY);
				}
				else {
					printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
				}
				
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}