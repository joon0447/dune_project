/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"


extern char backbuf[MAP_HEIGHT][MAP_WIDTH];
extern bool big_cursor;

// 표시할 색상 정의. 대충 맞춰 뒀는데, 취향껏 추가하거나 변경하기
#define COLOR_DEFAULT	7
#define COLOR_CURSOR	112
#define COLOR_RESOURCE  112
#define COLOR_BLUE 9
#define COLOR_GRAY 8
#define COLOR_ORANGE 6
#define COLOR_RED 4
#define COLOR_YELLOW 14
#define COLOR_BLACK 0
#define COLOR_WHITE 15
#define COLOR_GREEN 2
#define COLOR_OK 3
#define COLOR_PURPLE 5
#define COLOR_P 13

// 지금은 자원, 맵, 커서만 표시
// 앞으로 화면에 표시할 내용들 여기에 추가하기
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);

void object_info(char text[]);
void object_cmd(char text[]);
void object_cmd2(char text[], char text1[], char text2[], char text3[], char text4[]);
void print_system_message(char text[]);

#endif
