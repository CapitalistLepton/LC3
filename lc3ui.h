#ifndef LC3_UI_H
#define LC3_UI_H

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "lc3.h"

typedef struct cpu_s CPU_s;

void startUI();
void greetingWin();
void debugWin();
void displayDebug(CPU_s *cpu, int memStart, unsigned short mem[]);
void endUI();

#endif
