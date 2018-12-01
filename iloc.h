#ifndef ILOC_H
#define ILOC_H

#include "tree.h"

typedef enum {
    RBSS = -4,
    RFP,
    RSP,
    RPC
} SpecialRegister;

typedef enum {
    IADD = 1,
    SUB,
    MULT,
    DIV,
    ADDI,
    RSUBI,
    LOADI,
    LOADAI,
    STOREAI,
    I2I,
    CMP_EQ,
    CMP_LE,
    CMP_LT,
    CMP_GE,
    CMP_GT,
    CMP_NE,
    CBR,
    JUMPI,
    JUMP,
    HALT
} Mnemonic;

typedef struct instruction {
    Mnemonic mnemonic;
    int arg1;
    int arg2;
    int arg3;
    char* comment;
    int label;
    struct instruction* next;
} Instruction;

Instruction* math(BinOpType operation, int op1, int op2, int dst);
Instruction* addI(int op1, int op2, int dst);
Instruction* rsubI(int reg, int op, int dst);

Instruction* loadI(int value, int reg);
Instruction* loadAI(int src, int offset, int dst);
Instruction* storeAI(int src, int dst, int offset);
Instruction* i2i(int r1, int r2);

Instruction* compare(BinOpType comparison, int op1, int op2, int dst);
Instruction* cbr(int op, int label_true, int label_false);

Instruction* jumpI(int label);
Instruction* jump(int reg);

Instruction* halt();

void append_code(Instruction* i);
void set_main_label(int label);
void print_code();
char* reg_name(int reg);
Instruction* add_comment(char* comment);
Instruction* new_comment(char* comment);

#endif