
/*
20200908 ������

1) �غ� ~ 5) �ý��� �޽��� ���� �Ϸ�

3) �߸� ������ ���ʽ� 1,2�� ���� ����
4) ����1�� ���� �� ���ʽ��� ���� ����

*/

#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include "common.h"
#include "io.h"
#include "display.h"




void init(void);
void intro(void);
void outro(void);
void object_select(void);
void cursor_move(DIRECTION dir);
void cursor_double_move(DIRECTION dir);
void obj1_move(void);
void obj2_move(void);
void obj1_eat(void);
void obj2_eat(void);
bool obj1_dest(void);
bool obj2_dest(void);

void create_harvester(void);
void calc_count(void);

POSITION obj1_next_position(void);
POSITION obj2_next_position(void);

POSITION pos = { 1,0 };


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

SANDWORM obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT -4 , 1}, // �÷��̾� ���� �Ϻ�����
	.repr = 'W',
	.speed = 300,
	.next_move_time = 300
};

SANDWORM obj2 = {
	.pos = {10, 30},
	.dest = {MAP_HEIGHT - 15, MAP_WIDTH - 2}, // AI ���� �Ϻ����� 
	.repr = 'W',
	.speed = 300,
	.next_move_time = 300
};

int current_select = -1;
bool make_harve = false;
int count_harve = 0;

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor);

	KEY last_key = k_none; // �����Է� Ȯ���� ���� ������ �Է� Ű ���� ����
	int count = 0; //ª�� �ð� �����Է� �ν��� ���� ī��Ʈ ����


	while (1) {
		// loop �� ������(��, TICK==10ms����) Ű �Է� Ȯ��
		KEY key = get_key();

		//300ms �� ������ �Է� Ű �ʱ�ȭ
		count++;
		if (count == 15) { 
			count = 0;
			last_key = k_none;
		}

		// Ű �Է��� ������ ó��
		if (is_arrow_key(key)) {
			if (last_key == key) { // Ű �����Է�
				cursor_double_move(ktod(key));
				last_key = k_none;
			}
			else { // Ű �ѹ� �Է�
				cursor_move(ktod(key));
				last_key = key;
			}
	
		}
		else {
			// ����Ű ���� �Է�
			switch (key) {
			case k_quit: outro();
			case k_space: 
				object_select(); // ���� �Լ�
				break;
			case k_esc:
				object_info(" ");
				object_cmd(" ");
				current_select = -1;
				break;
			case k_h:
				if (current_select == 0) {
					create_harvester();
				}
				break;
			case k_none:
			case k_undef:
			default: break;
			}
		}


		obj1_move();
		obj2_move();

		// ȭ�� ���
		display(resource, map, cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("=== DUNE 1.5 ===\n");
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void calc_count() {
	if (make_harve) {
		count_harve++;
		if (count_harve >= 5) {
			print_system_message("�Ϻ����Ͱ� ����Ǿ����ϴ�.");
		}
		else {
			print_system_message("�Ϻ����Ͱ� ����Ǿ����ϴ�.");
			make_harve = false;
			count_harve = 0;
		}
	}
}

//������Ʈ ���� �Լ�
void object_select(void){
	POSITION curr = cursor.current;
	char ch = backbuf[curr.row][curr.column];
	if (ch == 'B') {  // ����
		if (curr.column < 5) {
			object_info("����");
			object_cmd("H : �Ϻ����� ����");
			current_select = 0;
		}
		else {
			object_info("����");
		}
	}
	else if (ch == 'P') { // ����
		object_info("����");
		object_cmd("");
		current_select = 1;
	}
	else if (ch == 'S') { // �����̽�
		object_info("�����̽�");
		object_cmd("");
		current_select = 2;
	}
	
	else if (ch == 'H') { // �Ϻ�����
		object_info("�Ϻ�����");
		object_cmd("");
	}

	else if (ch == 'W') { // �����
		object_info("�����");
		object_cmd("");
	}

	else if (ch == 'R') { // ����
		object_info("����");
		object_cmd("");
	}

	else {
		object_info("�縷 ����");
		object_cmd("");
	}
}

void init(void) {
	// layer 0(map[0])�� ���� ����
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}
	
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for(int j=0; j<MAP_WIDTH; j++){
			if (i >= MAP_HEIGHT - 3 && i < MAP_HEIGHT - 1 && j >= 1 && j < 3) { // �÷��̾� ����
				map[0][i][j] = 'B';
			}
			else if (i >= MAP_HEIGHT - 3 && i < MAP_HEIGHT - 1 && j >= 3 && j < 5) { // �÷��̾� ���� ���� ����
				map[0][i][j] = 'P';
			}
			else if (i == MAP_HEIGHT - 4 && j == 1) { // �÷��̾� ���� �Ϻ�����
				map[0][i][j] = 'H';
			}
			else if (i == MAP_HEIGHT - 6 && j == 1) { // �÷��̾� ���� �����̽�
				map[0][i][j] = 'S';
			}
			else if (i >= MAP_HEIGHT - 17 && i < MAP_HEIGHT - 15 && j >= MAP_WIDTH - 3 && j < MAP_WIDTH - 1) { // AI ����
				map[0][i][j] = 'B';
			}
			else if (i >= MAP_HEIGHT - 17 && i < MAP_HEIGHT - 15 && j >= MAP_WIDTH - 5 && j < MAP_WIDTH - 3) { // AI ����
				map[0][i][j] = 'P';
			}
			else if (i == MAP_HEIGHT - 15 && j == MAP_WIDTH - 2) { // AI �Ϻ�����
				map[0][i][j] = 'H';
			}
			else if (i == MAP_HEIGHT - 13 && j == MAP_WIDTH - 2) { // AI �����̽�
				map[0][i][j] = 'S';
			}
		}
	}

	// layer 1(map[1])�� ��� �α�(-1�� ä��)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	
}

// (�����ϴٸ�) ������ �������� Ŀ�� �̵�
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);

	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}
}

// �����Է� Ŀ�� �̵�
void cursor_double_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = curr;  // ���� ��ġ�� ���� ����

	// 3ĭ �̵��� ���� 3�� �̵� ���⿡ ���� �� ��ġ�� ���
	for (int i = 0; i < 3; i++) {
		new_pos = pmove(new_pos, dir);  // �� ĭ�� �̵�

		// �߰� �ܰ迡���� �Ź� validation check
		if (new_pos.row < 1 || new_pos.row > MAP_HEIGHT - 2 || \
			new_pos.column < 1 || new_pos.column > MAP_WIDTH - 2) {
			// ���� �̵� �Ұ��� ������ ������ �ߴ�
			break;
		}
	}

	// ���� ��ġ�� ��ȿ���� Ȯ�� �� Ŀ�� ��ġ ����
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}
}


void obj1_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// ���� �ð��� �� ����
		return;
	}
	
	obj1_eat();
	
	int percent = rand() % 100;
	// ������Ʈ(�ǹ�, ���� ��)�� layer1(map[1])�� ����
	
	if (percent < 10) {
		int num = rand() % 8 + 1;
		char spice = '0' + num;
		map[0][obj.pos.row][obj.pos.column] = spice;
	}
	obj.pos = obj1_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;
	obj.next_move_time = sys_clock + obj.speed;
}

// �����1 �Ϲ����� ��� �Ա�
void obj1_eat(void) {
	if (map[0][obj.pos.row][obj.pos.column] == 'H') { // �Ϻ����� ��� �Ա�
		map[0][obj.pos.row][obj.pos.column] = 'x';
		map[1][obj.pos.row][obj.pos.column] = -1;
		print_system_message("�Ϻ����Ͱ� ��������� ���߽��ϴ�.");
	}
	else {
		map[1][obj.pos.row][obj.pos.column] = -1;
	}
}

bool obj1_dest(void) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			char unit = map[0][i][j];
			// �Ϻ������� ���
			if (unit == 'H') {
				POSITION dest = { i,j };
				obj.dest = dest;
				return true;
			}
		}
	}
	return false;
}

POSITION obj1_next_position(void) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;


	// ������ ����. ������ �ܼ��� ���� �ڸ��� �պ�
	if (diff.row == 0 && diff.column == 0) {
		if (!obj1_dest()) {
			if (obj.dest.row == 1 && obj.dest.column == 1) {
				// topleft --> bottomright�� ������ ����
				POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
				obj.dest = new_dest;
			}
			else {
				// bottomright --> topleft�� ������ ����
				POSITION new_dest = { 1, 1 };
				obj.dest = new_dest;
			}
		}
		return obj.pos;
	}

	// ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}


	// validation check
	// next_pos�� ���� ����� �ʰ�, (������ ������)��ֹ��� �ε����� ������ ���� ��ġ�� �̵�
	// ������ �浹 �� �ƹ��͵� �� �ϴµ�, ���߿��� ��ֹ��� ���ذ��ų� ���� ������ �ϰų�... ���
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // ���ڸ�
	}
}

void obj2_move(void) {
	if (sys_clock <= obj2.next_move_time) {
		return;
	}
	obj2_eat();
	int percent = rand() % 100;
	if (percent < 10) {
		int num = rand() % 8 + 1;
		char spice = '0' + num;
		map[0][obj2.pos.row][obj2.pos.column] = spice;
	}
	obj2.pos = obj2_next_position();
	map[1][obj2.pos.row][obj2.pos.column] = obj2.repr;
	obj2.next_move_time = sys_clock + obj2.speed;
}

void obj2_eat(void) {
	if (map[0][obj2.pos.row][obj2.pos.column] == 'H') { // �Ϻ����� ��� �Ա�
		map[0][obj2.pos.row][obj2.pos.column] = 'x';
		map[1][obj2.pos.row][obj2.pos.column] = -1;
		print_system_message("�Ϻ����Ͱ� ��������� ���߽��ϴ�.");
	}
	else {
		map[1][obj2.pos.row][obj2.pos.column] = -1;
	}
}

bool obj2_dest(void) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			char unit = map[0][i][j];
			// �Ϻ������� ���
			if (unit == 'H') {
				POSITION dest = { i,j };
				obj2.dest = dest;
				return true;
			}
		}
	}
	return false;
}



POSITION obj2_next_position(void) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub(obj2.dest, obj2.pos);
	DIRECTION dir;

	// ������ ����. ������ �ܼ��� ���� �ڸ��� �պ�
	if (diff.row == 0 && diff.column == 0) {
		if (!obj2_dest()) {
			if (obj2.dest.row == 1 && obj2.dest.column == 1) {
				// topleft --> bottomright�� ������ ����
				POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
				obj2.dest = new_dest;
			}
			else {
				// bottomright --> topleft�� ������ ����
				POSITION new_dest = { 1, 1 };
				obj2.dest = new_dest;
			}
		}
		return obj2.pos;
	}

	// ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos�� ���� ����� �ʰ�, (������ ������)��ֹ��� �ε����� ������ ���� ��ġ�� �̵�
	// ������ �浹 �� �ƹ��͵� �� �ϴµ�, ���߿��� ��ֹ��� ���ذ��ų� ���� ������ �ϰų�... ���
	POSITION next_pos = pmove(obj2.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj2.pos;  // ���ڸ�
	}
}

void create_harvester() {
	if (resource.spice >= 5) {
		print_system_message("�Ϻ����Ͱ� ���� �Ǿ����ϴ�.");
	}
	else {
		print_system_message("�Ϻ����� ���꿡 �ʿ��� �����̽��� �����մϴ�.");
	}
	
}