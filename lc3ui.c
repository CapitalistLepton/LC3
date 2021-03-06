#include "lc3ui.h"

/*
 * Zane Littrell, Luke Gillmore, Trenton Greevebiester, Veronica Gross
 * ncurses UI for the lc3
 */ 

WINDOW *greeting;
WINDOW *debug;
WINDOW *selection;
WINDOW *input;
WINDOW *output;
WINDOW *halt;

void startUI() {
  initscr(); // Init curses
  keypad(stdscr, TRUE); // Use keyboard mapping
  cbreak(); // Read in all characters without waiting for an ENTER
  
  char *msg = "Welcome to the LC-3 Simulator";
  int leftX = (COLS - strlen(msg)) / 2;
  greetingWin(msg);
  debugWin(leftX);
  selectWin(leftX);
  inputWin(leftX);
  outputWin(leftX);
  halt = newwin(OUT_LEN, COLS, DEBUG_LEN + SELECT_LEN + IN_LEN + OUT_LEN, leftX);

  wrefresh(stdscr);
  wrefresh(greeting);
  wrefresh(debug);
  wrefresh(selection);
  wrefresh(input);
  wrefresh(output);
  wrefresh(halt);
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
  wprintw(selection, "Select: 1) Load, 2) Run, 3) Step, 5) Display Mem, 9) Exit\n");
  wprintw(selection, "> ");
}

void inputWin(int leftX) {
  input = newwin(IN_LEN, COLS, DEBUG_LEN + SELECT_LEN, leftX);
  wprintw(input, "Input: \n");
}

void outputWin(int leftX) {
  output = newwin(OUT_LEN, COLS, DEBUG_LEN + SELECT_LEN + IN_LEN, leftX);
  wprintw(output, "Output: ");
}

void displayDebug(CPU_s *cpu, ALU_s *alu, int memStart, unsigned short mem[]) {
  int i;

  for (i = 0; i < NUM_REG; i++) {
    mvwprintw(debug, GREET_LEN + i, 0, "R%d:x%04X\t\tx%04X:x%04X", i, 
      cpu->regFile[i], memStart + i + MEM_START, mem[memStart + i]);
  }
  for (; i < 11; i++) {
    mvwprintw(debug, GREET_LEN + i, 0, "\t\t\tx%04X:x%04X", memStart + i + MEM_START,
      mem[memStart + i]);
  }
  mvwprintw(debug, GREET_LEN + i, 0, "PC:x%04X  IR:x%04X\tx%04X:x%04X", cpu->pc + MEM_START,
    cpu->ir, memStart + i + MEM_START, mem[memStart + i]);
  i++;
  mvwprintw(debug, GREET_LEN + i, 0, "A: x%04X  B: x%04X\tx%04X:x%04X", alu->a, 
    alu->b, memStart + i + MEM_START, mem[memStart + i]);
  i++;
  mvwprintw(debug, GREET_LEN + i, 0, "MAR:x%04X MDR:x%04X\tx%04X:x%04X", 
    cpu->mar + MEM_START, cpu->mdr, memStart + i + MEM_START, mem[memStart + i]);
  i++;
  mvwprintw(debug, GREET_LEN + i, 0, "CC: N:%X Z:%X P:%X\t\tx%04X:x%04X", 
    cpu->n, cpu->z, cpu->p, memStart + i + MEM_START, mem[memStart + i]);
  i++;
  mvwprintw(debug, GREET_LEN + i, 0, "\t\t\tx%04X:x%04X", memStart + i + MEM_START,
      mem[memStart + i]);
  wrefresh(debug);
}

char getSelection() {
  char choice;
  wmove(selection, SELECT_CUR_Y, SELECT_CUR_X);
  wclrtoeol(selection);
  choice = wgetch(selection);
  return choice;
}

void clearOutput() {
  wclear(output);
  wprintw(output, "Output: ");
  wrefresh(output);
}

void putString(char *str) {
  wprintw(output, "%s", str);
  wrefresh(output);
}

void outChar(char ch) {
  wprintw(output, "%c", ch);
  wrefresh(output);
}

void displayHalt() {
  wprintw(halt, "------HALTING PROGRAM------");
  wrefresh(halt);
}

void clearHalt() {
  wclear(halt);
  wrefresh(halt);
}

char getChar() {
  wclear(input);
  wprintw(input, "Input: ");
  wrefresh(input);
  wmove(input, IN_CUR_Y, IN_CUR_X);
  char ch = wgetch(input);
  return ch;
}

void getString(char *str, int length) {
  wclear(input);
  wprintw(input, "Input: ");
  wrefresh(input);
  char ch;
  int i;
  for (i = 0; i < length; i++) {
    str[i] = '\0';
  }
  i = 0;
  wmove(input, IN_CUR_Y, IN_CUR_X);
  ch = wgetch(input);
  str[i++] = ch;
  while(ch != '\n') {
    ch = wgetch(input);
    str[i++] = ch;
  }
  str[--i] = '\0';
}

void endUI() {
  delwin(greeting);
  delwin(debug);
  endwin();
}
