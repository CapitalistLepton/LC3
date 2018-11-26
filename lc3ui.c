#include "lc3ui.h"

WINDOW *greeting;
WINDOW *debug;
WINDOW *selection;
WINDOW *input;
WINDOW *ouput;

void startUI() {
  initscr(); // Init curses
  keypad(stdscr, TRUE); // Use keyboard mapping
  cbreak(); // Read in all characters without waiting for an ENTER

  greetingWin();
  debugWin();

  wrefresh(stdscr);
  wrefresh(greeting);
  wrefresh(debug);
}

void greetingWin() {
  char *msg = "Welcome to the LC-3 Simulator";
  greeting = newwin(1, COLS, 0, (COLS - strlen(msg)) / 2);
  wprintw(greeting, msg);
}

void debugWin() {
  char *label = "Registers\t\tMemory";
  debug = newwin(18, COLS, 1, 10);
  wprintw(debug, label);
}

void displayDebug(CPU_s *cpu, int memStart, unsigned short mem[]) { 
  int i;

  for (i = 0; i < 8; i++) {
    mvwprintw(debug, 1 + i, 0, "R%d:x%X\t\tx%X:x%X", i, cpu->regFile[i], memStart + i, 
      mem[memStart + i]);
  }
  wrefresh(debug);
}

void endUI() {
  delwin(greeting);
  delwin(debug);
  endwin();
}
