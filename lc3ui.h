#ifndef LC3_UI_H
#define LC3_UI_H

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "lc3.h"

#define NUM_REG 8
#define MEM_LINES 11
#define GREET_LEN 1
#define DEBUG_LEN 18
#define SELECT_LEN 2
#define IN_LEN 1
#define OUT_LEN 1
#define SELECT_SHIFT -7
#define SELECT_CUR_X 2
#define SELECT_CUR_Y 1
#define IN_CUR_X 8
#define IN_CUR_Y 0

typedef struct cpu_s CPU_s;
typedef struct alu_s ALU_s;

void startUI();
void greetingWin(char *msg);
void debugWin(int leftX);
void selectWin(int leftX);
void inputWin(int leftX);
void outputWin(int leftX);
void displayDebug(CPU_s *cpu, ALU_s *alu, int memStart, unsigned short mem[]);
char getSelection();
void putString(char *str);
void outChar(char ch);
char getChar();
void endUI();

#endif
