/*
* raw(?) I/O
*/
#include "io.h"

void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row }; // 행, 열 반대로 전달
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void set_color(int color) { // 글자 색상 함수
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void set_bgColor(int text, int back) { // 배경색 포함 색상
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text | (back<<4));
}

void printc(POSITION pos, char ch, int color) {
	if (color >= 0) {
		set_color(color);
	}
	gotoxy(pos);
	printf("%c", ch);
}

void print_info(POSITION pos, char ch[], int color) {
	if (color >= 0) {
		set_color(color);
	}
	gotoxy(pos);
	printf("%s", ch);
}


// 배경색 변경
void printBgc(POSITION pos, char ch, int color, int back) {
	if (color >= 0) {
		set_bgColor(color, back);
	}
	gotoxy(pos);
	printf("%c", ch);
}

KEY get_key(void) {
	if (!_kbhit()) {  // 입력된 키가 있는지 확인
		return k_none;
	}

	int byte = _getch();    // 입력된 키를 전달 받기
	switch (byte) {
	case 'q': return k_quit;  // 'q'를 누르면 종료
	case 32: //스페이스바
		return k_space;
	case 27: //ESC
		return k_esc;
	case 72:
	case 104:
		return k_h;
	case 88:
	case 120:
		return k_x;
	case 98:
	case 66:
		return k_b;
	case 112:
	case 80:
		return k_p;
	case 100:
	case 68:
		return k_d;
	case 103:
	case 71:
		return k_g;
	case 115:
	case 83:
		return k_s;
	case 109:
	case 77:
		return k_m;
	case 102:
	case 70:
		return k_f;
	case 224:
		byte = _getch();  // MSB 224가 입력 되면 1바이트 더 전달 받기
		switch (byte) {
		case 72: return k_up;
		case 75: return k_left;
		case 77: return k_right;
		case 80: return k_down;
		default: return k_undef;
		}
	default: return k_undef;
	}
}
