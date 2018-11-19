#include "lc3ui.h"

WINDOW *greeting;
WINDOW *debug;
WINDOW *selection;
WINDOW *input;
WINDOW *ouput;

void startUI() {
  char *msg = "Welcome to the LC-3 Simulator";
  int leftX;

  initscr(); // Init curses
  keypad(stdscr, TRUE); // Use keyboard mapping
  cbreak(); // Read in all characters without waiting for an ENTER
  
  leftX = (COLS - strlen(msg)) / 2;
  greetingWin(msg);
  debugWin(leftX);
  selectWin(leftX);

  wrefresh(stdscr);
  wrefresh(greeting);
  wrefresh(debug);
  wrefresh(selection);
}

void greetingWin(char *msg) {
  greeting = newwin(GREET_LEN, COLS, 0, (COLS - strlen(msg)) / 2);
  wprintw(greeting, msg);
}

void debugWin(int leftX) {
  char *label = "Registers\t\tMemory";
  debug = newwin(DEBUG_LEN, COLS, GREET_LEN, leftX);
  wprintw(debug, label);
}

void selectWin(int leftX) {
  selection = newwin(SELECT_LEN, COLS, DEBUG_LEN, leftX + SELECT_SHIFT);
  wprintw(selection, "Select: 1) Load, 3) Step, 5) Display Mem, 9) Exit\n");
  wprintw(selection, "> ");
}

void displayDebug(CPU_s *cpu, ALU_s *alu, int memStart, unsigned short mem[]) {
  int i;

  for (i = 0; i < 8; i++) {
    mvwprintw(debug, GREET_LEN + i, 0, "R%d:x%04X\t\tx%04X:x%04X", i, 
      cpu->regFile[i], memStart + i, mem[memStart + i]);
  }
  for (; i < 11; i++) {
    mvwprintw(debug, GREET_LEN + i, 0, "\t\t\tx%04X:x%04X", i, memStart + i,
      mem[memStart + i]);
  }
  mvwprintw(debug, GREET_LEN + i, 0, "PC:x%04X  IR:x%04X\tx%04X:x%04X", cpu->pc,
    cpu->ir, memStart + i, mem[memStart + i]);
  i++;
  mvwprintw(debug, GREET_LEN + i, 0, "A: x%04X  B: x%04X\tx%04X:x%04X", alu->a, 
    alu->b, memStart + i, mem[memStart + i]);
  i++;
  mvwprintw(debug, GREET_LEN + i, 0, "MAR:x%04X MDR:x%04X\tx%04X:x%04X", 
    cpu->mar, cpu->mdr, memStart + i, mem[memStart + i]);
  i++;
  mvwprintw(debug, GREET_LEN + i, 0, "CC: N:%X Z:%X P:%X\t\tx%04X:x%04X", 
    cpu->n, cpu->z, cpu->p, memStart + i, mem[memStart + i]);
  i++;
  mvwprintw(debug, GREET_LEN + i, 0, "\t\t\tx%04X:x%04X", i, memStart + i,
      mem[memStart + i]);
  wrefresh(debug);
}

char getSelection() {
  wmove(selection, SELECT_CUR_Y, SELECT_CUR_X);
  char choice = wgetch(selection);
  return choice;
}

void putString(char *str) {
}

void outChar(char ch) {
}

char getChar() {
  return 0;
}

void endUI() {
  delwin(greeting);
  delwin(debug);
  endwin();
}
