#ifndef LC3_H
#define LC3_H

#include <stdio.h>
#include <stdlib.h>
#include "lc3ui.h"

#define NUM_REGISTERS 8
#define SIZE_OF_MEM 128 
#define MAX_STR_LEN 100

#define SIGN_BIT_9 0x100
#define SIGN_EXTEND_9 0xFE00
#define SIGN_BIT_5 0x10
#define SIGN_EXTEND_5 0xFFE0
#define SIGN_BIT_6 0x20
#define SIGN_EXTEND_6 0xFFC0
#define SIGN_BIT_11 0x400
#define SIGN_EXTEND_11 0xF800

#define LAST3 0x7
#define LAST5 0x1F
#define LAST6 0x3F
#define LAST8 0xFF
#define LAST9 0x1FF
#define LAST11 0x7FF

#define R7 7
#define ZERO 0

#define ADD_IMMED 0x20
#define JSR_IMMED 0x800

#define OPCODE_SHIFT 12
#define DR_SHIFT 9 
#define SR1_SHIFT 6

#define BR 0
#define ADD 1
#define LD 2
#define JSR 4
#define ST 3
#define AND 5
#define LDR 6
#define STR 7
#define NOT 9
#define JMP 12
#define LEA 14
#define TRAP 15

#define GETC 32
#define OUT 33
#define PUTS 34
#define HALT 37

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

Register sext(Register reg, Register signBit, Register signExtend);
Register sext9(Register reg);
Register sext6(Register reg);
Register sext5(Register reg);
void run(CPU_s *cpu, ALU_s *alu);
int runStep(CPU_s *cpu, ALU_s *alu);
void load(char *filename);
void nzpCheck(CPU_s *cpu, Register reg);
void trapPuts(CPU_s *cpu);

#endif
