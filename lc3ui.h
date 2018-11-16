#ifndef LC3_UI_H
#define LC3_UI_H

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "lc3.h"

typedef struct cpu_s CPU_s;
typedef struct alu_s ALU_s;

void startUI();
void greetingWin(char *msg);
void debugWin(int leftX);
void displayDebug(CPU_s *cpu, ALU_s *alu, int memStart, unsigned short mem[]);
void putString(char *str);
void outChar(char ch);
char getChar();
void endUI();

#endif
