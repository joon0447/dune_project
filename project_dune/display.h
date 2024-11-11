/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"


extern char backbuf[MAP_HEIGHT][MAP_WIDTH];


// ǥ���� ���� ����. ���� ���� �״µ�, ���ⲯ �߰��ϰų� �����ϱ�
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

// ������ �ڿ�, ��, Ŀ���� ǥ��
// ������ ȭ�鿡 ǥ���� ����� ���⿡ �߰��ϱ�
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor
);

void object_info(char text[]);
void object_cmd(char text[]);
void print_system_message(char text[]);

#endif
