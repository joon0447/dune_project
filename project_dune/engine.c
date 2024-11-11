
/*
20200908 조상준

1) 준비 ~ 5) 시스템 메시지 까지 완료

3) 중립 유닛의 보너스 1,2는 하지 않음
4) 유닛1개 생산 의 보너스는 하지 않음

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
	.dest = {MAP_HEIGHT -4 , 1}, // 플레이어 본진 하베스터
	.repr = 'W',
	.speed = 300,
	.next_move_time = 300
};

SANDWORM obj2 = {
	.pos = {10, 30},
	.dest = {MAP_HEIGHT - 15, MAP_WIDTH - 2}, // AI 본진 하베스터 
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

	KEY last_key = k_none; // 더블입력 확인을 위한 마지막 입력 키 저장 변수
	int count = 0; //짧은 시간 더블입력 인식을 위한 카운트 변수


	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		//300ms 후 마지막 입력 키 초기화
		count++;
		if (count == 15) { 
			count = 0;
			last_key = k_none;
		}

		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			if (last_key == key) { // 키 더블입력
				cursor_double_move(ktod(key));
				last_key = k_none;
			}
			else { // 키 한번 입력
				cursor_move(ktod(key));
				last_key = key;
			}
	
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_quit: outro();
			case k_space: 
				object_select(); // 선택 함수
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

		// 화면 출력
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
			print_system_message("하베스터가 생산되었습니다.");
		}
		else {
			print_system_message("하베스터가 생산되었습니다.");
			make_harve = false;
			count_harve = 0;
		}
	}
}

//오브젝트 선택 함수
void object_select(void){
	POSITION curr = cursor.current;
	char ch = backbuf[curr.row][curr.column];
	if (ch == 'B') {  // 본진
		if (curr.column < 5) {
			object_info("본진");
			object_cmd("H : 하베스터 생산");
			current_select = 0;
		}
		else {
			object_info("본진");
		}
	}
	else if (ch == 'P') { // 장판
		object_info("장판");
		object_cmd("");
		current_select = 1;
	}
	else if (ch == 'S') { // 스파이스
		object_info("스파이스");
		object_cmd("");
		current_select = 2;
	}
	
	else if (ch == 'H') { // 하베스터
		object_info("하베스터");
		object_cmd("");
	}

	else if (ch == 'W') { // 샌드웜
		object_info("샌드웜");
		object_cmd("");
	}

	else if (ch == 'R') { // 바위
		object_info("바위");
		object_cmd("");
	}

	else {
		object_info("사막 지형");
		object_cmd("");
	}
}

void init(void) {
	// layer 0(map[0])에 지형 생성
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
			if (i >= MAP_HEIGHT - 3 && i < MAP_HEIGHT - 1 && j >= 1 && j < 3) { // 플레이어 본진
				map[0][i][j] = 'B';
			}
			else if (i >= MAP_HEIGHT - 3 && i < MAP_HEIGHT - 1 && j >= 3 && j < 5) { // 플레이어 본진 우측 장판
				map[0][i][j] = 'P';
			}
			else if (i == MAP_HEIGHT - 4 && j == 1) { // 플레이어 본진 하베스터
				map[0][i][j] = 'H';
			}
			else if (i == MAP_HEIGHT - 6 && j == 1) { // 플레이어 본진 스파이스
				map[0][i][j] = 'S';
			}
			else if (i >= MAP_HEIGHT - 17 && i < MAP_HEIGHT - 15 && j >= MAP_WIDTH - 3 && j < MAP_WIDTH - 1) { // AI 본진
				map[0][i][j] = 'B';
			}
			else if (i >= MAP_HEIGHT - 17 && i < MAP_HEIGHT - 15 && j >= MAP_WIDTH - 5 && j < MAP_WIDTH - 3) { // AI 장판
				map[0][i][j] = 'P';
			}
			else if (i == MAP_HEIGHT - 15 && j == MAP_WIDTH - 2) { // AI 하베스터
				map[0][i][j] = 'H';
			}
			else if (i == MAP_HEIGHT - 13 && j == MAP_WIDTH - 2) { // AI 스파이스
				map[0][i][j] = 'S';
			}
		}
	}

	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	
}

// (가능하다면) 지정한 방향으로 커서 이동
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

// 더블입력 커서 이동
void cursor_double_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = curr;  // 현재 위치를 먼저 복사

	// 3칸 이동을 위해 3번 이동 방향에 따라 새 위치를 계산
	for (int i = 0; i < 3; i++) {
		new_pos = pmove(new_pos, dir);  // 한 칸씩 이동

		// 중간 단계에서도 매번 validation check
		if (new_pos.row < 1 || new_pos.row > MAP_HEIGHT - 2 || \
			new_pos.column < 1 || new_pos.column > MAP_WIDTH - 2) {
			// 만약 이동 불가한 범위가 나오면 중단
			break;
		}
	}

	// 최종 위치가 유효한지 확인 후 커서 위치 갱신
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}
}


void obj1_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}
	
	obj1_eat();
	
	int percent = rand() % 100;
	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	
	if (percent < 10) {
		int num = rand() % 8 + 1;
		char spice = '0' + num;
		map[0][obj.pos.row][obj.pos.column] = spice;
	}
	obj.pos = obj1_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;
	obj.next_move_time = sys_clock + obj.speed;
}

// 샌드웜1 일반유닛 잡아 먹기
void obj1_eat(void) {
	if (map[0][obj.pos.row][obj.pos.column] == 'H') { // 하베스터 잡아 먹기
		map[0][obj.pos.row][obj.pos.column] = 'x';
		map[1][obj.pos.row][obj.pos.column] = -1;
		print_system_message("하베스터가 샌드웜에게 당했습니다.");
	}
	else {
		map[1][obj.pos.row][obj.pos.column] = -1;
	}
}

bool obj1_dest(void) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			char unit = map[0][i][j];
			// 하베스터일 경우
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
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;


	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (!obj1_dest()) {
			if (obj.dest.row == 1 && obj.dest.column == 1) {
				// topleft --> bottomright로 목적지 설정
				POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
				obj.dest = new_dest;
			}
			else {
				// bottomright --> topleft로 목적지 설정
				POSITION new_dest = { 1, 1 };
				obj.dest = new_dest;
			}
		}
		return obj.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}


	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
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
	if (map[0][obj2.pos.row][obj2.pos.column] == 'H') { // 하베스터 잡아 먹기
		map[0][obj2.pos.row][obj2.pos.column] = 'x';
		map[1][obj2.pos.row][obj2.pos.column] = -1;
		print_system_message("하베스터가 샌드웜에게 당했습니다.");
	}
	else {
		map[1][obj2.pos.row][obj2.pos.column] = -1;
	}
}

bool obj2_dest(void) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			char unit = map[0][i][j];
			// 하베스터일 경우
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
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj2.dest, obj2.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (!obj2_dest()) {
			if (obj2.dest.row == 1 && obj2.dest.column == 1) {
				// topleft --> bottomright로 목적지 설정
				POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
				obj2.dest = new_dest;
			}
			else {
				// bottomright --> topleft로 목적지 설정
				POSITION new_dest = { 1, 1 };
				obj2.dest = new_dest;
			}
		}
		return obj2.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj2.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj2.pos;  // 제자리
	}
}

void create_harvester() {
	if (resource.spice >= 5) {
		print_system_message("하베스터가 생산 되었습니다.");
	}
	else {
		print_system_message("하베스터 생산에 필요한 스파이스가 부족합니다.");
	}
	
}