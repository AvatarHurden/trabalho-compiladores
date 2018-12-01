#include "iloc.h"

#include <stdlib.h>
#include <stdio.h>
#include "string.h"

Instruction* first_instruction = NULL;
Instruction* last_instruction = NULL;

Instruction* new_instruction(Mnemonic mnemonic, int arg1, int arg2, int arg3) {
    Instruction* i = malloc(sizeof(Instruction));
    i->mnemonic = mnemonic;
    i->arg1 = arg1;
    i->arg2 = arg2;
    i->arg3 = arg3;
    i->label = -1;
    i->comment = NULL;
    i->next = NULL;
    append_code(i);
    return i;
}

void append_code(Instruction* i) {
    if (!first_instruction) {
        first_instruction = i;
    } else {
        last_instruction->next = i;
    }
    last_instruction = i;
}

Instruction* math(BinOpType operation, int op1, int op2, int dst) {
    Mnemonic m;
    switch(operation) {
    case ADD: m = IADD; break;
    case SUBTRACT: m = SUB; break;
    case MULTIPLY: m = MULT; break;
    case DIVIDE: m = DIV; break;
    default: break;
    }
    return new_instruction(m, op1, op2, dst);
}
Instruction* addI(int op1, int op2, int dst) {
    return new_instruction(ADDI, op1, op2, dst);
}
Instruction* rsubI(int reg, int op, int dst) {
    return new_instruction(RSUBI, reg, op, dst);
}
Instruction* loadI(int value, int reg) {
    return new_instruction(LOADI, value, reg, 0);
}
Instruction* loadAI(int src, int offset, int dst) {
    return new_instruction(LOADAI, src, offset, dst);
}
Instruction* storeAI(int src, int dst, int offset) {
    return new_instruction(STOREAI, src, dst, offset);
}
Instruction* i2i(int r1, int r2) {
    return new_instruction(I2I, r1, r2, 0);
}

Instruction* compare(BinOpType comparison, int op1, int op2, int dst) {
    Mnemonic m;
    switch(comparison) {
    case EQUAL: m = CMP_EQ; break;
    case GREATER: m = CMP_GT; break;
    case GREATER_EQUAL: m = CMP_GE; break;
    case LESS_EQUAL: m = CMP_LE; break;
    case LESS_THAN: m = CMP_LT; break;
    case NOT_EQUAL: m = CMP_NE; break;
    default: break;
    }
    return new_instruction(m, op1, op2, dst);
}
Instruction* cbr(int op, int label_true, int label_false) {
    return new_instruction(CBR, op, label_true, label_false);
}

Instruction* jumpI(int label) {
    return new_instruction(JUMPI, label, 0, 0);
}
Instruction* jump(int reg) {
    return new_instruction(JUMP, reg, 0, 0);
}

Instruction* halt() {
    return new_instruction(HALT, 0, 0, 0);
}

void set_main_label(int label) {
    Instruction* i = first_instruction;
    while (i) {
        if (i->mnemonic == JUMPI) {
            i->arg1 = label;
            break;
        }
        i = i->next;
    }
}

void print_code() {
    Instruction* i = first_instruction;
    while(i) {
        if (i->label >= 0) printf("L%d: ", i->label);
        switch(i->mnemonic) {
        case LOADI:
            if (i->arg2 < 0)
                printf("loadI %d => %s", i->arg1, reg_name(i->arg2));
            else
                printf("loadI %d => r%d", i->arg1, i->arg2);
            break;
        case LOADAI:
            printf("loadAI %s, %d => r%d", reg_name(i->arg1), i->arg2, i->arg3);
            break;
        case JUMPI:
            printf("jumpI -> L%d", i->arg1);
            break;
        case JUMP:
            printf("jump -> r%d", i->arg1);
            break;
        case I2I:
            if (i->arg1 < 0)
                if (i->arg2 < 0)
                    printf("i2i %s => %s", reg_name(i->arg1), reg_name(i->arg2));
                else
                    printf("i2i %s => r%d", reg_name(i->arg1), i->arg2);
            else
                if (i->arg2 < 0)
                    printf("i2i r%d => %s", i->arg1, reg_name(i->arg2));
                else
                    printf("i2i r%d => r%d", i->arg1, i->arg2);
            break;
        case ADDI:
            if (i->arg3 < 0)
                printf("addI %s, %d => %s", reg_name(i->arg1), i->arg2, reg_name(i->arg3));
            else 
                printf("addI %s, %d => r%d", reg_name(i->arg1), i->arg2, i->arg3);
            break;
        case RSUBI:
            printf("rsubI r%d, %d => r%d", i->arg1, i->arg2, i->arg3);
            break;
        case STOREAI:
            printf("storeAI r%d => %s, %d", i->arg1, reg_name(i->arg2), i->arg3);
            break;
        case CBR:
            printf("cbr r%d -> L%d, L%d", i->arg1, i->arg2, i->arg3);
            break;
        case IADD:
            printf("add r%d, r%d => r%d", i->arg1, i->arg2, i->arg3);
            break;
        case SUB:
            printf("sub r%d, r%d => r%d", i->arg1, i->arg2, i->arg3);
            break;
        case MULT:
            printf("mult r%d, r%d => r%d", i->arg1, i->arg2, i->arg3);
            break;
        case DIV:
            printf("div r%d, r%d => r%d", i->arg1, i->arg2, i->arg3);
            break;
        case CMP_EQ:
            printf("cmp_EQ r%d, r%d -> r%d", i->arg1, i->arg2, i->arg3);
            break;
        case CMP_GE:
            printf("cmp_GE r%d, r%d -> r%d", i->arg1, i->arg2, i->arg3);
            break;
        case CMP_GT:
            printf("cmp_GT r%d, r%d -> r%d", i->arg1, i->arg2, i->arg3);
            break;
        case CMP_LE:
            printf("cmp_LE r%d, r%d -> r%d", i->arg1, i->arg2, i->arg3);
            break;
        case CMP_LT:
            printf("cmp_LT r%d, r%d -> r%d", i->arg1, i->arg2, i->arg3);
            break;
        case CMP_NE:
            printf("cmp_NE r%d, r%d -> r%d", i->arg1, i->arg2, i->arg3);
            break;
        case HALT:
            printf("halt");
            break;
        default:
            break;
        }
        if (i->comment) printf(" // %s", i->comment);
        printf("\n");
        i = i->next;
    }
}

char* reg_name(int reg) {
    switch (reg) {
        case RFP: return "rfp";
        case RSP: return "rsp";
        case RBSS: return "rbss";
        case RPC: return "rpc";
        default: return "";
    }
}

Instruction* add_comment(char* comment) {
    if (last_instruction->comment) {
        new_instruction(0, 0, 0, 0);
    }
    last_instruction->comment = comment;
    return last_instruction;
}

Instruction* new_comment(char* comment) {
    Instruction* i = new_instruction(0, 0, 0, 0);
    i->comment = comment;
    return i;
}