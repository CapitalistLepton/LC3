#include "lc3.h"
#include "lc3ui.h"

/*
 * Zane Littrell, Luke Gillmore, Trenton Greevebiester
 * Final Project
 */

unsigned short mem[SIZE_OF_MEM];

int main(int argc, char *argv[]) {
  startUI();

  int i;
  int line = 0;
  CPU_s *cpu = (CPU_s *) malloc(sizeof(CPU_s));
  ALU_s *alu = (ALU_s *) malloc(sizeof(ALU_s));
  FILE *in = fopen(argv[1], "r");

  cpu->pc = 0;
  cpu->ir = 0;
  cpu->sext = 0;
  cpu->mar = 0;
  cpu->mdr = 0;
  for (i = 0; i < NUM_REGISTERS; i++) {
    cpu->regFile[i] = 0;
  }
  // Read in memory
  while (line < SIZE_OF_MEM && fscanf(in, "0x%hX\n", &mem[line++]) != EOF);
  displayDebug(cpu, alu, 0, mem);
  getch();
  endUI();
//  controller(cpu, alu);

  return 0;
}

int controller(CPU_s *cpu, ALU_s *alu) {
  unsigned short opcode, dr, sr1, sr2, nzp, immed5, PCoffest9;
  unsigned char n, z, p; n = z = p = 0;
  int state = FETCH;

  for (;;) {
    switch (state) {
      case FETCH:
        printf("Here in FETCH\n");
        cpu->mar = cpu->pc;
        cpu->pc++;
        cpu->mdr = mem[cpu->mar];
        cpu->ir = cpu->mdr;
        state = DECODE;
        break;
      case DECODE:
        printf("Here in DECODE\n");
        opcode = cpu->ir >> OPCODE_SHIFT;
        state = EVAL_ADDR;
        break;
      case EVAL_ADDR:
        printf("Here in EVAL_ADDR\n");
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
            PCoffest9 = cpu->ir & LAST9;
            cpu->sext = sext(PCoffest9, SIGN_BIT_9, SIGN_EXTEND_9);
            sr1 = cpu->sext;
            alu->a = cpu->pc;
            alu->b = sr1;
            alu->r = alu->a + alu->b;
			nzpCheck(cpu, r);
            break;
          case ST:
            sr1 = cpu->ir >> DR_SHIFT & LAST3;
             PCoffest9 = cpu->ir & LAST9;
             cpu->sext = sext(PCoffest9, SIGN_BIT_9, SIGN_EXTEND_9);
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
			nzpCheck(cpu, r);
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
            PCoffest9 = cpu->ir & LAST9;
            cpu->sext = sext(PCoffest9, SIGN_BIT_9, SIGN_EXTEND_9);
            sr1 = cpu->sext;
            alu->a = cpu->pc;
            alu->b = sr1;
            alu->r = alu->a + alu->b;
			nzpCheck(cpu, r);
            break;
		  case JSR:
			if (cpu->ir & JSR_IMMED) { 
              cpu->sext = sext11(cpu->ir & LAST11);
            } else {
			  sr1 = cpu->ir >> SR1_SHIFT & LAST3;
            }
            break;
            case BR:

              nzp = cpu->ir >> DR_SHIFT & LAST3;
                PCoffest9 = cpu->ir & LAST9;
                cpu->sext = sext(PCoffest9, SIGN_BIT_9, SIGN_EXTEND_9);
                n = (nzp & 4) ?  1 : 0;
                z = (nzp & 2) ?  1 : 0;
                p = (nzp & 1) ?  1 : 0;

            case TRAP:
              cpu->pc = cpu->ir & TRAPVECT8;


              break;
        }
        state = FETCH_OP;
        break;
      case FETCH_OP:
        printf("Here in FETCH_OP\n");
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
          case TRAP:
            if (cpu->pc == TRAPVECT8) {
              printf("%s\n","HALTING PROGRAM");
              exit(0);
            }
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
        printf("Here in EXECUTE\n");
        switch (opcode) {
          case ADD:
            alu->r = alu->a + alu->b;
			nzpCheck(cpu, r);
            break;
          case AND:
            alu->r = alu->a & alu->b;
			nzpCheck(cpu, r);
            break;
          case NOT:
            alu->r = ~alu->a;
			nzpCheck(cpu, r);
            break;
		  case JSR:
			cpu->regFile[R7] = cpu->pc;
            alu->r = alu->a + alu->b;
			cpu->pc = alu->r;
            break;
        }
        state = STORE;
        break;
      case STORE:
        printf("Here in STORE\n");
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
    printStatus(cpu, alu);
    if (cpu->pc >= SIZE_OF_MEM) { // HALT at end of memory
      exit(0);
    }
  }
}

void printStatus(CPU_s *cpu, ALU_s *alu) {
  int i;
  printf("Registers:\n");
  for (i = 0; i < NUM_REGISTERS; i++) {
    printf("R%d:  0x%04X\n", i, cpu->regFile[i]);
  }
  printf("PC:  0x%04X\n", cpu->pc);
  printf("IR:  0x%04X\n", cpu->ir);
  printf("SEXT:0x%04X\n", cpu->sext);
  printf("MAR: 0x%04X\n", cpu->mar);
  printf("MDR: 0x%04X\n", cpu->mdr);
  printf("A:   0x%04X\n", alu->a);
  printf("B:   0x%04X\n", alu->b);
  printf("R:   0x%04X\n", alu->r);
  printf("\n");
  printf("Memory:\n");
  printf("Addr:   Contents:\n");
  for (i = 0; i < SIZE_OF_MEM; i++) {
    printf("0x%04X  0x%04X\n", i, mem[i]);
  }
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
