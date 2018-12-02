#include "lc3.h"
#include "lc3ui.h"

/*
 * Zane Littrell, Luke Gillmore, Trenton Greevebiester
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
        getString(filename, MAX_STR_LEN);
        load(filename);
        putString("");
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
  unsigned short opcode, dr, sr1, sr2, nzp, immed5, PCoffset;
  unsigned char n, z, p; n = z = p = 0;
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
              immed5 = cpu->ir & LAST5;
              cpu->sext = sext(immed5, SIGN_BIT_5, SIGN_EXTEND_5);
              sr2 = cpu->sext;
            } else {
              sr2 = cpu->ir & LAST3;
            }
            break;
          case LD:
            dr = cpu->ir >> DR_SHIFT & LAST3;
            PCoffset = cpu->ir & LAST9;
            cpu->sext = sext(PCoffset, SIGN_BIT_9, SIGN_EXTEND_9);
            sr1 = cpu->sext;
            alu->a = cpu->pc;
            alu->b = sr1;
            alu->r = alu->a + alu->b;
            nzpCheck(cpu, alu->r);
            break;
          case ST:
            sr1 = cpu->ir >> DR_SHIFT & LAST3;
            PCoffset = cpu->ir & LAST9;
            cpu->sext = sext(PCoffset, SIGN_BIT_9, SIGN_EXTEND_9);
            alu->a = cpu->pc;
            alu->b = cpu->sext;
            alu->r = alu->a + alu->b;
            break;
          case AND:
            dr = cpu->ir >> DR_SHIFT & LAST3;
            sr1 = cpu->ir >> SR1_SHIFT & LAST3;
            if (cpu->ir & ADD_IMMED) { // ADD and AND use same bit to show immediate values
              immed5 = cpu->ir & LAST5;
              cpu->sext = sext(immed5, SIGN_BIT_5, SIGN_EXTEND_5);
              sr2 = cpu->sext;
            } else {
              sr2 = cpu->ir & LAST3;
            }
            break;
          case LDR:
            dr = cpu->ir >> DR_SHIFT & LAST3;
            sr1 = cpu->ir >> SR1_SHIFT & LAST3;
            immed5 = cpu->ir & LAST6;
            cpu->sext = sext(immed5, SIGN_BIT_6, SIGN_EXTEND_6);
            sr2 = cpu->sext;
            alu->a = cpu->regFile[sr1];
            alu->b = sr2;
            alu->r = alu->a + alu->b;
            nzpCheck(cpu, alu->r);
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
            PCoffset = cpu->ir & LAST9;
            cpu->sext = sext(PCoffset, SIGN_BIT_9, SIGN_EXTEND_9);
            sr1 = cpu->sext;
            alu->a = cpu->pc;
            alu->b = sr1;
            alu->r = alu->a + alu->b;
            nzpCheck(cpu, alu->r);
            break;
          case JSR:
            if (cpu->ir & JSR_IMMED) { 
              PCoffset = cpu->ir & LAST11;
              cpu->sext = sext(PCoffset, SIGN_BIT_11, SIGN_EXTEND_11);
            } else {
              sr1 = cpu->ir >> SR1_SHIFT & LAST3;
            }
            break;
          case BR:
            nzp = cpu->ir >> DR_SHIFT & LAST3;
            PCoffset = cpu->ir & LAST9;
            cpu->sext = sext(PCoffset, SIGN_BIT_9, SIGN_EXTEND_9);
            n = nzp & 4;
            z = nzp & 2;
            p = nzp & 1;
          case TRAP:
            cpu->regFile[7] = cpu->pc;
            cpu->pc = cpu->ir & LAST8;
            break;
          case STR:
            sr1 = cpu->ir >> DR_SHIFT & LAST3;
            sr2 = cpu->ir >> SR1_SHIFT & LAST3;
            PCoffset = cpu->ir & LAST6;
            cpu->sext = sext(PCoffset, SIGN_BIT_6, SIGN_EXTEND_6);
            alu->a = cpu->regFile[sr2];
            alu->b = cpu->sext;
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
          case STR:
            cpu->mar = alu->r;
            cpu->mdr = cpu->regFile[sr1];
            break;
          case AND:
            alu->a = cpu->regFile[sr1];
            if(cpu->ir & ADD_IMMED) {
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
          case BR:
            if( (n & cpu->n) || (z & cpu->z) || (p & cpu->p) ) {
              cpu->pc = cpu->sext + cpu->pc;
            }
            cpu->n = cpu->z = cpu->p = n = z = p = 0;
            break;
          case JSR:
            if (cpu->ir & JSR_IMMED) { 
              alu->a = cpu->pc;
              alu->b = cpu->sext;
            } else {
              alu->a = ZERO;
              alu->b = cpu->regFile[sr1];
            }
        }
        state = EXECUTE;
        break;
      case EXECUTE:
        switch (opcode) {
          case ADD:
            alu->r = alu->a + alu->b;
            nzpCheck(cpu, alu->r);
            break;
          case AND:
            alu->r = alu->a & alu->b;
            nzpCheck(cpu, alu->r);
            break;
          case NOT:
            alu->r = ~alu->a;
            nzpCheck(cpu, alu->r);
            break;
          case JSR:
            cpu->regFile[R7] = cpu->pc;
            alu->r = alu->a + alu->b;
            cpu->pc = alu->r;
            break;
          case TRAP:
            switch(cpu->pc) {
              case GETC:
                sr1 = getChar();
                cpu->regFile[0] = sr1;
                break;
              case OUT:
                outChar(cpu->regFile[0]);
                break;
              case PUTS:
                trapPuts(cpu);
                break; 
              case HALT:
                //putString("-----HALTING PROGRAM-----");
                return HALT;
            }
            cpu->pc = cpu->regFile[7];
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
          case STR:
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
  while(line < SIZE_OF_MEM && fscanf(in, "%hX\n", &mem[line++]) != EOF);

  fclose(in);
}

void nzpCheck(CPU_s *cpu, Register reg) {
  if (reg < 0) 
    cpu->n = 1;
  else
    cpu->n = 0;
  if (reg > 0) 
    cpu->p = 1;
  else
    cpu->p = 0;
  if (reg == 0) 
    cpu->z = 1;
  else
    cpu->z = 0;
}

/*
 * Input the register, the signed bit to check, and the
 * amount of the signed extension that is required.
 */
Register sext(Register reg, Register signBit, Register signExtend) {

  Register out = reg;
  if (reg & signBit) {
    out |= signExtend; // Set first 9 bits is negative
  }
  return out;
}

void trapPuts(CPU_s *cpu) {
  int index = cpu->regFile[0];
  char in[50] = "\0";
  while(mem[index] != 0) {
    in[index] = mem[index];
    index++;
  }
  putString(in);
  cpu->pc = cpu->regFile[7];
}
