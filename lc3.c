#include "lc3.h"

/*
 * Zane Littrell, Luke Gillmore, Trenton Greevebiester, Veronica Gross
 * Final Project
 */

unsigned short mem[SIZE_OF_MEM];

int main(int argc, char *argv[]) {
  int i;
  CPU_s *cpu = (CPU_s *) malloc(sizeof(CPU_s));
  ALU_s *alu = (ALU_s *) malloc(sizeof(ALU_s));
  char filename[MAX_STR_LEN];

  cpu->pc = 0;
  cpu->ir = 0;
  cpu->sext = 0;
  cpu->mar = 0;
  cpu->mdr = 0;
  cpu->n = 0;
  cpu->z = 0;
  cpu->p = 0;
  for (i = 0; i < NUM_REGISTERS; i++) {
    cpu->regFile[i] = 0;
  }

  startUI();
  for (;;) {
    char sel = getSelection();
    switch (sel) {
      case '1':
        putString("Enter the filename above");
        getString(filename, MAX_STR_LEN); // TODO fix this
        load("input.txt"); // TODO use filename here
        cpu->pc = 0;
        break;
      case '2':
        run(cpu, alu);
        break;
      case '3':
        runStep(cpu, alu);
        break;
      case '5':
        displayDebug(cpu, alu, cpu->pc, mem);
        break;
      case '9':
        endUI();
        return 0;
    }
  }
}

void run(CPU_s *cpu, ALU_s *alu) {
  while(runStep(cpu, alu) != HALT);
}

int runStep(CPU_s *cpu, ALU_s *alu) {
  unsigned short opcode, dr, sr1, sr2;
  int state = FETCH;
  do {
    switch (state) {
      case FETCH:
        cpu->mar = cpu->pc;
        cpu->pc++;
        cpu->mdr = mem[cpu->mar];
        cpu->ir = cpu->mdr;
        state = DECODE;
        break;
      case DECODE:
        opcode = cpu->ir >> OPCODE_SHIFT;
        state = EVAL_ADDR;
        break;
      case EVAL_ADDR:
        switch (opcode) {
          case ADD:
            dr = cpu->ir >> DR_SHIFT & LAST3;
            sr1 = cpu->ir >> SR1_SHIFT & LAST3;
            if (cpu->ir & ADD_IMMED) {
              cpu->sext = sext5(cpu->ir & LAST5);
              sr2 = cpu->sext;
            } else {
              sr2 = cpu->ir & LAST3;
            }
            break;
          case LD:
            dr = cpu->ir >> DR_SHIFT & LAST3;
            cpu->sext = sext9(cpu->ir & LAST9);
            sr1 = cpu->sext;
            alu->a = cpu->pc;
            alu->b = sr1;
            alu->r = alu->a + alu->b;
            break;
          case ST:
            sr1 = cpu->ir >> DR_SHIFT & LAST3;
            cpu->sext = sext9(cpu->ir & LAST9);
            alu->a = cpu->pc;
            alu->b = cpu->sext;
            alu->r = alu->a + alu->b;
            break;
          case AND:
            dr = cpu->ir >> DR_SHIFT & LAST3;
            sr1 = cpu->ir >> SR1_SHIFT & LAST3;
            if (cpu->ir & ADD_IMMED) { // ADD and AND use same bit to show immediate values
              cpu->sext = sext5(cpu->ir & LAST5);
              sr2 = cpu->sext;
            } else {
              sr2 = cpu->ir & LAST3;
            }
            break;
          case LDR:
            dr = cpu->ir >> DR_SHIFT & LAST3;
            sr1 = cpu->ir >> SR1_SHIFT & LAST3;
            cpu->sext = sext6(cpu->ir & LAST6);
            sr2 = cpu->sext;
            alu->a = cpu->regFile[sr1];
            alu->b = sr2;
            alu->r = alu->a + alu->b;
            break;
          case NOT:
            dr = cpu->ir >> DR_SHIFT & LAST3;
            sr1 = cpu->ir >> SR1_SHIFT & LAST3;
            break;
          case JMP:
            sr1 = cpu->ir >> SR1_SHIFT & LAST3;
            break;
          case LEA:
            dr = cpu->ir >> DR_SHIFT & LAST3;
            cpu->sext = sext9(cpu->ir & LAST9);
            sr1 = cpu->sext;
            alu->a = cpu->pc;
            alu->b = sr1;
            alu->r = alu->a + alu->b;
            break;
        }
        state = FETCH_OP;
        break;
      case FETCH_OP:
        switch (opcode) {
          case ADD:
            alu->a = cpu->regFile[sr1];
            if (cpu->ir & ADD_IMMED) {
              alu->b = sr2;
            } else {
              alu->b = cpu->regFile[sr2];
            }
            break;
          case LD:
          case LDR:
            cpu->mar = alu->r;
            cpu->mdr = mem[cpu->mar];
            break;
          case ST:
            cpu->mar = alu->r;
            cpu->mdr = cpu->regFile[sr1];
            break;
          case AND:
            alu->a = cpu->regFile[sr1];
            if(cpu->ir & ADD_IMMED){
                alu->b = sr2;
            } else {
                alu->b = cpu->regFile[sr2];
            }
            break;
          case NOT:
            alu->a = cpu->regFile[sr1];
            break;
          case JMP:
            cpu->pc = cpu->regFile[sr1];
            break;
        }
        state = EXECUTE;
        break;
      case EXECUTE:
        switch (opcode) {
          case ADD:
            alu->r = alu->a + alu->b;
            break;
          case AND:
            alu->r = alu->a & alu->b;
            break;
          case NOT:
            alu->r = ~alu->a;
            break;
          case TRAP:
            switch(cpu->ir & LAST8) {
              case GETC:
                sr1 = getChar();
                cpu->regFile[0] = sr1;
                break;
              case OUT:
                outChar(cpu->regFile[0]);
              case HALT:
                return HALT;
            }
            break;
        }
        state = STORE;
        break;
      case STORE:
        switch (opcode) {
          case ADD:
          case AND:
          case NOT:
          case LEA:
            cpu->regFile[dr] = alu->r;
            break;
          case LD:
          case LDR:
            cpu->regFile[dr] = cpu->mdr;
            break;
          case ST:
            mem[cpu->mar] = cpu->mdr;
            break;
        }
        state = FETCH;
        break;
    }
  } while (state != FETCH); 
  return 0;
}

void load(char *filename) {
  FILE *in = fopen(filename, "r");
  char str[10];
  int line = 0;
  while(line < SIZE_OF_MEM && fscanf(in, "0x%hX\n", &mem[line++]) != EOF);

  fclose(in);
}

Register sext5(Register reg) {
  Register out = reg;
  if (reg & SIGN_BIT_5) {
    out |= SIGN_EXTEND_5;
  }
  return out;
}

Register sext6(Register reg) {
  Register out = reg;
  if (reg & SIGN_BIT_6) {
    out |= SIGN_EXTEND_6;
  }
  return out;
}

Register sext9(Register reg) {
  Register out = reg;
  if (reg & SIGN_BIT_9) {
    out |= SIGN_EXTEND_9;
  }
  return out;
}
