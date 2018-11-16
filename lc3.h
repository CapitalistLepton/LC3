#ifndef LC3_H
#define LC3_H

#include <stdio.h>
#include <stdlib.h>
#include "lc3ui.h"

#define NUM_REGISTERS 8
#define SIZE_OF_MEM 32

#define SIGN_BIT_9 0x200
#define SIGN_EXTEND_9 0xFC00
#define SIGN_BIT_5 0x20
#define SIGN_EXTEND_5 0xFFC0
#define SIGN_BIT_6 0x40
#define SIGN_EXTEND_6 0xFF80

#define LAST3 0x7
#define LAST5 0x1F
#define LAST6 0x3F
#define LAST9 0x1FF

#define ADD_IMMED 0x20

#define OPCODE_SHIFT 12
#define DR_SHIFT 9 
#define SR1_SHIFT 6

#define ADD 1
#define LD 2
#define ST 3
#define AND 5
#define LDR 6
#define NOT 9
#define JMP 12
#define LEA 14

#define FETCH 0
#define DECODE 1
#define EVAL_ADDR 2
#define FETCH_OP 3
#define EXECUTE 4
#define STORE 5

typedef unsigned short Register;

typedef struct alu_s {
  Register a;
  Register b;
  Register r;
} ALU_s;

typedef struct cpu_s {
  Register regFile[NUM_REGISTERS];
  Register ir;
  Register sext;
  Register mar;
  Register mdr;
  Register pc;
  unsigned char n;
  unsigned char z;
  unsigned char p;
} CPU_s;

Register sext9(Register reg);
Register sext6(Register reg);
Register sext5(Register reg);
int controller(CPU_s *cpu, ALU_s *alu);
void printStatus(CPU_s *cpu, ALU_s *alu);

#endif
