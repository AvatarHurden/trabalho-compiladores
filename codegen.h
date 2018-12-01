#ifndef CODEGEN_H
#define CODEGEN_H

#include "table.h"
#include "iloc.h"

typedef struct memory {
    char* id;
    SpecialRegister base_reg;
    int offset;
    struct memory* next;
} Memory;

typedef struct function_info {
    char* id;
    int label;
    struct function_info* next;
} FunctionInfo;

void init_iloc();
void generate_code(Node* node);
void expression_code(Node* node);
void global_var_code(GlobalVarNode var_node);
void local_var_code(LocalVarNode var_node);
void attr_code(AttrNode attr_node);
void int_code(int int_node);
void var_access_code(VariableNode var_node);

void un_op_code(UnOpNode node);
void bin_op_code(BinOpNode node);
void logic_expression(BinOpNode node);
void relational_expression(BinOpNode node);
void arithmetic_expression(BinOpNode node);

void if_code(IfNode if_node);
void while_code(WhileNode while_node);
void do_while_code(WhileNode do_while_node);

void function_decl_code(FunctionDeclNode func_decl_node);
int function_params_code(ParamNode* params);
void function_call_code(FunctionCallNode func_call_node);
int function_args_code(FunctionCallNode func_call_node);
void return_code(ListNode return_node);

int function_arg_count(Node* args);

void new_memory(char* id, SpecialRegister base_reg, int offset);
Memory* find_memory(char* id);

FunctionInfo* new_function_info(char* id, int label);
FunctionInfo* find_function_info(char* id);

void push_scope();
void pop_scope();
int new_reg();
int new_label();

#endif