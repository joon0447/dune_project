
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
void object_build(int build);
void cursor_move(DIRECTION dir);
void cursor_double_move(DIRECTION dir);
void obj1_move(void);
void obj2_move(void);
void obj1_eat(void);
void obj2_eat(void);
bool obj1_dest(void);
bool obj2_dest(void);
bool is_spice_there(int row, int column);
void work_harvester(void);
void unit_move(UNIT* unit, int id);

void move_marin();
void create_harvester(void);
void calc_count(void);
void create_harvester_pointer(int row, int column);


POSITION obj1_next_position(void);
POSITION obj2_next_position(void);

POSITION pos = { 1,0 };

UNIT* curr_unit;


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 10,
	.spice_max = 0,
	.population = 0,
	.population_max = 10
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

UNIT* unit_map[MAP_HEIGHT][MAP_WIDTH]; // 유닛 정보 저장

ORDER_UNIT order_unit = {
	.row = 0,
	.column = 0,
};

int current_select = -1;
/*
-1 : 기본 모드
-2 : 건설 모드

*/
bool make_harve = false;
int count_harve = 0;

bool s_flag = true;

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
				if (current_select == -3) {
					object_build(0); // 장판
				}
				else if (current_select == -4) {
					// 숙소
					object_build(1);
				}
				else if (current_select == -5) {
					//창고
					object_build(2);
				}
				else if (current_select == -6) {
					//병영
					object_build(3);
				}
				else if (current_select == -7) {
					//은신처
					object_build(4);
				}
				else {
					object_select(); // 선택 함수
				}
				break;
			case k_esc:
				object_info(" ");
				object_cmd(" ");
				if (current_select == -2 || current_select == -3) {
					print_system_message("건물 건설 모드를 종료합니다.                          ");
				}
				else if (current_select == 10) {
					print_system_message("스파이스 선택 모드를 종료합니다.                          ");
				}
				current_select = -1;
				big_cursor = false;
				break;
			case k_h:
				if (current_select == 0) {
					create_harvester();
				}
				else if (current_select == 3) { // harvest
					print_system_message("스파이스 매장지를 선택해주세요.                          ");
					current_select = 10;
				}
				big_cursor = false;
				break;
			case k_b:
				if (current_select == -1) {
					current_select = -2;
					big_cursor = false;
					object_cmd2("P: Plate", "D: Dormitory", "G: Garage", "B: Barracks", "S: Shelter", "ESC : Cancel");
					print_system_message("건물 건설 모드를 시작합니다.                          ");
				}
				else if (current_select == -2) {
					big_cursor = true;
					print_system_message("건설할 장판을 선택해주세요.                          ");
					current_select = -6;
				}

				break;
			case k_p: //장판
				if (current_select == -2) {
					current_select = -3;
					big_cursor = true;	
				}
				else if (current_select = 6) {

				}
				break;
			case k_d: // 숙소
				if (current_select == -2) {
					big_cursor = true;
					print_system_message("건설할 장판을 선택해주세요.                          ");
					current_select = -4;
				}
				break;
			case k_g: // 창고
				if (current_select == -2) {
					big_cursor = true;
					print_system_message("건설할 장판을 선택해주세요.                          ");
					current_select = -5;
				}
				break;
			case k_m:
				if (current_select == 6) {
					print_system_message("이동할 장소를 선택해주세요.                         ");
					if (curr_unit != NULL) {
						curr_unit->work_type = 0;
					}
				}
			case k_s:
				if (current_select == -2) {
					big_cursor = true;
					print_system_message("건설할 장판을 선택해주세요.                                ");
					current_select = -7;
				}
				else if (current_select == 4) { // 보병 생산
					s_flag = true;
					if (resource.population_max - resource.population >= 1) {
						if (resource.spice >= 5) {
							for (int i = 0; i < MAP_HEIGHT; i++) {
								for (int j = 0; j < MAP_WIDTH; j++) {
									char back = backbuf[i][j];
									if (back != '#') {
										map[0][i][j] = 'o';
										resource.spice -= 5;
										resource.population += 1;

										UNIT* marin = (UNIT*)malloc(sizeof(UNIT));
										if (!marin) {
											print_system_message("보병 생성에 실패했습니다 : 메모리 부족      ");
											return;
										}

										POSITION pos = { i, j };
										marin->pos = pos;
										marin->start_pos = pos;
										marin->move_period = 400;
										marin->attack = 15;
										marin->attack_period = 200;
										marin->max_hp = 25;
										marin->hp = 25;
										marin->sight = 8;
										marin->id = 1;
										unit_map[pos.row][pos.column] = marin;
										print_system_message("보병이 생산되었습니다.                   ");
										s_flag = false;
										break;
									}
								}
								if (!s_flag) {
									break;
								}
							}
							if (s_flag == true) {
								print_system_message("생성할 공간이 부족합니다.                     ");
							}
						}
						else {
							print_system_message("스파이스가 부족합니다.                              ");
						}
					}
					else {
						print_system_message("인구수가 부족합니다.                              ");
					}
				}
				break;
			case k_none:
			case k_undef:
			default: break;
			}
		}

		if (current_select == -1) {
			object_cmd("B: Build");
		}

		move_marin();


		obj1_move();
		obj2_move();
		
		work_harvester();
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
	for (int i = 0; i < 100; i++) {
		printf(" \n");
	}
	printf("=== 게임 종료!! ==\n");
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

void object_build(int build) {
	POSITION x = { 1,0 };
	POSITION y = { 0,1 };
	POSITION xy = { 1,1 };

	POSITION curr = cursor.current;
	POSITION curr_a = padd(curr, x);
	POSITION curr_b = padd(curr, y);
	POSITION curr_c = padd(curr, xy);
	char ch = backbuf[curr.row][curr.column];

		if (ch == 'p') {
			if (build == 1) {
				//숙소
				if (resource.spice >= 2) {
					map[0][curr.row][curr.column] = 'd';
					map[0][curr_a.row][curr_a.column] = 'D';
					map[0][curr_b.row][curr_b.column] = 'D';
					map[0][curr_c.row][curr_c.column] = 'D';
					print_system_message("숙소 건설을 완료했습니다.                ");
					resource.population_max += 10;
					resource.spice -= 2;
				}
				else {
					print_system_message("스파이스가 부족합니다.                ");
				}
				current_select = -1;
				big_cursor = false;
			}
			else if (build == 2) {
				//창고
				if (resource.spice >= 4) {
					map[0][curr.row][curr.column] = 'g';
					map[0][curr_a.row][curr_a.column] = 'G';
					map[0][curr_b.row][curr_b.column] = 'G';
					map[0][curr_c.row][curr_c.column] = 'G';
					print_system_message("창고 건설을 완료했습니다.               ");
					resource.spice_max += 10;
					resource.spice -= 4;
				}
				else {
					print_system_message("스파이스가 부족합니다.                ");
				}
				current_select = -1;
				big_cursor = false;
			}
			else if (build == 3) {
				//병영
				if (resource.spice > +4) {
					map[0][curr.row][curr.column] = 'a';
					map[0][curr_a.row][curr_a.column] = 'A';
					map[0][curr_b.row][curr_b.column] = 'A';
					map[0][curr_c.row][curr_c.column] = 'A';
					resource.spice -= 4;
					print_system_message("병영 건설을 완료했습니다.                 ");
				}
				else {
					print_system_message("스파이스가 부족합니다.                ");
				}
				current_select = -1;
				big_cursor = false;
			}
			else if (build == 4) {
				// 은신처
				if (resource.spice >= 5) {
					map[0][curr.row][curr.column] = 'r';
					map[0][curr_a.row][curr_a.column] = 'z';
					map[0][curr_b.row][curr_b.column] = 'z';
					map[0][curr_c.row][curr_c.column] = 'z';
					resource.spice -= 5;
					print_system_message("은신처 건설을 완료했습니다.                ");
				}
				else {
					print_system_message("스파이스가 부족합니다.                ");
				}
				current_select = -1;
				big_cursor = false;
			}
		}
		else {
			if (build != 0) {
				print_system_message("건물은 장판에만 건설할 수 있습니다.             ");
			}
			
		}
	
	if (current_select == -3) { // 장판 건설
		if (curr.row == MAP_HEIGHT - 2) {
			print_system_message("공간이 부족합니다.                ");
		}
		else {
			if (resource.spice >= 1) {
				resource.spice -= 1;
				map[0][curr.row][curr.column] = 'p';
				map[0][curr_a.row][curr_a.column] = 'P';
				map[0][curr_b.row][curr_b.column] = 'P';
				map[0][curr_c.row][curr_c.column] = 'P';
				print_system_message("장판 건설을 완료했습니다.            ");
			}
			else {
				print_system_message("스파이스가 부족합니다.                ");
			}
			current_select = -1;
			big_cursor = false;
		}
	}
}

//오브젝트 선택 함수
void object_select(void){
	POSITION curr = cursor.current;
	char ch = backbuf[curr.row][curr.column];
	if (current_select != 10) {
		if (ch == 'B') {  // 본진
			if (current_select != -2 && current_select != -3) {
				if (curr.column < 5) {
					object_info("본진");
					object_cmd("H : 하베스터 생산");
					current_select = 0;
				}
				else {
					object_info("본진");
				}
			}

		}
		else if (ch == 'P') { // 장판
			object_info("장판");
			object_cmd("");
			big_cursor = false;
			current_select = 1;
		}
		else if (ch == 'S') { // 스파이스
			
			if (current_select != -2 && current_select != -3) {
				object_info("스파이스");
				object_cmd("");
				current_select = 2;
			}
		}

		else if (ch == 'H') { // 하베스터
			if (current_select != -2 && current_select != -3) {
				UNIT* unit = unit_map[curr.row][curr.column];

				order_unit.row = curr.row;
				order_unit.column = curr.column;

				char buffer[100];
				sprintf_s(buffer, sizeof(buffer), "하베스터 : 체력 %d / %d", unit->hp, unit->max_hp);
				object_info(buffer);
				object_cmd("H: Harverst, M: Move");
				current_select = 3;
			}

		}

		else if (ch == 'h') {
			if (current_select != -2 && current_select != -3) {
				UNIT* unit = unit_map[curr.row][curr.column];
				char buffer[100];
				sprintf_s(buffer, sizeof(buffer), "하베스터 : 체력 %d / %d", unit->hp, unit->max_hp);
				object_info(buffer);
				object_cmd("");
			}

		}

		else if (ch == 'W') { // 샌드웜
			if (current_select != -2 && current_select != -3) {
				object_info("샌드웜");
				object_cmd("");
			}

		}

		else if (ch == 'R') { // 바위
			if (current_select != -2 && current_select != -3) {
				object_info("바위");
				object_cmd("");
			}

		}

		else if (ch == 'a' || ch == 'A') { // 병영
			object_info("병영");
			object_cmd("S: Soldier");
			current_select = 4;
		}

		else if (ch == 'r' || ch == 'z') {
			object_info("은신처");
			object_cmd("F: Fremen");
			current_select = 5;
		}

		else if (ch == 'o') {
			object_info("보병");
			object_cmd("M: Move , P : Patrol");
			current_select = 6;
			curr_unit = unit_map[curr.row][curr.column];
		}

		else {
			if (current_select != -2 && current_select != -3) {
				object_info("사막 지형");
				object_cmd("");
				if (current_select == 6) {
					curr_unit->des = curr;
					curr_unit->work = true;
					print_system_message("보병이 선택 장소로 이동합니다.                       ");
				}
				current_select = -1;
			}
			
		}
	}
	else if(current_select == 10) { // 하베스터 수확
		if (ch == 'S' || (ch >= 49 && ch <=57)) { 
			POSITION des = cursor.current;
			UNIT* unit = unit_map[order_unit.row][order_unit.column];
			int spice = (int)backbuf[cursor.current.row][cursor.current.column];
			if (spice >= 4) {
				int random_value = (rand() % 3) + 2;
				unit->take_spice = random_value;
			}
			else {
				unit->take_spice = spice;
			}
			unit->des = des;
			unit->work = true;
			unit->spice_pos = des;
			print_system_message("하베스터가 스파이스 수확을 시작합니다.     ");
			current_select = -1;
		}
		else {
			print_system_message("스파이스 지형을 선택해주세요.          ");
			print_system_message("취소하려면 ESC를 눌러주세요.           ");
		}
	}
}

void init(void) {
	//unit_map 초기화
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			unit_map[i][j] = NULL;
		}
	}


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
				if (i == MAP_HEIGHT - 3 && j == 3) {
					map[0][i][j] = 'p';
				}
				else {
					map[0][i][j] = 'P';
				}
			}
			else if (i == MAP_HEIGHT - 4 && j == 1) { // 플레이어 본진 하베스터
				create_harvester_pointer(i, j);
				map[0][i][j] = 'H';
			}
			else if (i == MAP_HEIGHT - 6 && j == 1) { // 플레이어 본진 스파이스
				map[0][i][j] = 'S';
			}
			else if (i >= MAP_HEIGHT - 17 && i < MAP_HEIGHT - 15 && j >= MAP_WIDTH - 3 && j < MAP_WIDTH - 1) { // AI 본진
				map[0][i][j] = 'B';
			}
			else if (i >= MAP_HEIGHT - 17 && i < MAP_HEIGHT - 15 && j >= MAP_WIDTH - 5 && j < MAP_WIDTH - 3) { // AI 장판
				if (i == MAP_HEIGHT - 17 && j == MAP_WIDTH - 5) {
					map[0][i][j] = 'p';
				}
				else {
					map[0][i][j] = 'P';
				}
			}
			else if (i == MAP_HEIGHT - 15 && j == MAP_WIDTH - 2) { // AI 하베스터
				create_harvester_pointer(i, j);
				map[0][i][j] = 'h';
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
	if (map[0][obj.pos.row][obj.pos.column] == 'R') {
		POSITION newPos = { obj.pos.row + 1, obj.pos.column };
		obj.pos = newPos;
	}
	map[1][obj.pos.row][obj.pos.column] = obj.repr;
	obj.next_move_time = sys_clock + obj.speed;
}

// 샌드웜1 일반유닛 잡아 먹기
void obj1_eat(void) {
	if (map[0][obj.pos.row][obj.pos.column] == 'H' || map[0][obj.pos.row][obj.pos.column] == 'h') { // 하베스터 잡아 먹기
		map[0][obj.pos.row][obj.pos.column] = 'x';
		map[1][obj.pos.row][obj.pos.column] = -1;
		print_system_message("하베스터가 샌드웜에게 당했습니다.                ");
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
			if (unit == 'H' || unit == 'h') {
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

	if (map[0][obj2.pos.row][obj2.pos.column] == 'R') {
		POSITION newPos = { obj2.pos.row + 1, obj2.pos.column };
		obj2.pos = newPos;
	}
	map[1][obj2.pos.row][obj2.pos.column] = obj2.repr;
	obj2.next_move_time = sys_clock + obj2.speed;
}

void obj2_eat(void) {
	if (map[0][obj2.pos.row][obj2.pos.column] == 'H' || map[0][obj2.pos.row][obj2.pos.column] == 'h') { // 하베스터 잡아 먹기
		map[0][obj2.pos.row][obj2.pos.column] = 'x';
		map[1][obj2.pos.row][obj2.pos.column] = -1;
		print_system_message("하베스터가 샌드웜에게 당했습니다.               ");
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
			if (unit == 'H' || unit == 'h') {
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
		if (resource.population_max - resource.population >= 5) {
			int a = 0;
			int b = 0;
			//i == MAP_HEIGHT - 4 && j == 1
			for (int i = 1; i < MAP_WIDTH; i++) {
				if (backbuf[MAP_HEIGHT - 4][i] != 'H') {
					map[0][MAP_HEIGHT - 4][i] = 'H';
					a = MAP_HEIGHT - 4;
					b = i;
					break;
				}
			}
			create_harvester_pointer(a, b);
			resource.spice -= 5;
			resource.population += 5;
			print_system_message("하베스터가 생산 되었습니다.                   ");
		}
		else {
			print_system_message("숙소 내 공간이 부족합니다.                   ");
		}
	}
	else {
		print_system_message("하베스터 생산에 필요한 스파이스가 부족합니다.");
	}
}

void create_harvester_pointer(int row, int column) {
	UNIT* harvester = (UNIT*)malloc(sizeof(UNIT));
	if (!harvester) {
		print_system_message("하베스터 생성에 실패했습니다 : 메모리 부족      ");
		return;
	}
	POSITION pos = { row, column };
	harvester->pos = pos;
	harvester->work = false;
	harvester->population = 5;
	harvester->move_period = 200;
	harvester->next_move_time = 0;
	harvester->attack = 0;
	harvester->attack_period = 0;
	harvester->hp = 70;
	harvester->max_hp = 70;
	harvester->sight = 0;
	harvester->start_pos = pos;
	harvester->id = 0;
	harvester->hold_time = 0;
	harvester->take_spice = 0;
	unit_map[pos.row][pos.column] = harvester;
}

bool is_spice_there(int row, int column) {
	return map[0][row][column] == 'S';
}

void work_harvester() {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			UNIT* unit = unit_map[i][j];
			if (unit != NULL) {
				if (unit->id == 0) {
					if (unit->work) {
						if (sys_clock <= unit->next_move_time) {
							return;
						}
						unit_move(unit, 0);
					}
				}
			}
			
		}
	}
}

void move_marin() {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			UNIT* unit = unit_map[i][j];
			if (unit != NULL) {
				if (unit->id == 1) {
					if (unit->work) {
						if (sys_clock <= unit->next_move_time) {
							return;
						}
						unit_move(unit, 1);
					}
				}
			}

		}
	}
}

void unit_move(UNIT* unit, int id) {
	POSITION diff = psub(unit->des, unit->pos);
	DIRECTION dir;

	// 목적지 도착
	if (diff.row == 0 && diff.column == 0) {
		POSITION des = unit->des;
		POSITION start_pos = unit->start_pos;
		
		if (id == 0) { // 하베스터
			if(des.row == start_pos.row && des.column == start_pos.column) { // 스파이스 먹으러 가기
				if (backbuf[start_pos.row][start_pos.column] == 'S' || (backbuf[start_pos.row][start_pos.column] >= 49 && backbuf[start_pos.row][start_pos.column] <= 57)) {
					unit->des = unit->spice_pos;
				}
			}
			else { // 본진으로 복귀하기
				if (unit->hold_time == 0) {
					unit->hold_time = sys_clock;
					return;
				}

				if (sys_clock - unit->hold_time < 1000) {
					return;
				}

				unit->hold_time = 0;
				int spice = unit->take_spice;
				if (spice + resource.spice > resource.spice_max) {
					resource.spice = resource.spice_max;
				}
				else {
					resource.spice += spice;
				}
				unit->des = unit->start_pos;
				unit->take_spice = 0;
			}
		}

		//보병

		else if (id == 1) {

		}
		return;
	}

	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	POSITION curr_pos = unit->pos;
	POSITION next_pos = pmove(unit->pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

	}
	else {
		// 제자리
		next_pos = unit->pos;
	}

	// 하베스터
	if (id == 0) {
		map[0][curr_pos.row][curr_pos.column] = 'x';
		map[0][next_pos.row][next_pos.column] = 'H';
		unit_map[curr_pos.row][curr_pos.column] = NULL;
		unit_map[next_pos.row][next_pos.column] = unit;
		unit->pos = next_pos;
		unit->next_move_time = sys_clock + unit->move_period;
	}

	//보병
	else if (id == 1) {
		map[0][curr_pos.row][curr_pos.column] = 'x';
		map[0][next_pos.row][next_pos.column] = 'o';
		unit_map[curr_pos.row][curr_pos.column] = NULL;
		unit_map[next_pos.row][next_pos.column] = unit;
		unit->pos = next_pos;
		unit->next_move_time = sys_clock + unit->move_period;
	}
}