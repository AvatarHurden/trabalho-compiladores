#include "table.h"

typedef struct memory {
    char* id;
    char* base_reg;
    int offset;
    struct memory* next;
} Memory;

void generate_code(Node* node);
void global_var_code(GlobalVarNode var_node);
void local_var_code(LocalVarNode var_node);
void attr_code(AttrNode attr_node);
void int_code(int int_node);
void var_access_code(VariableNode var_node);
void expression_code(Node* node);
void bin_op_code(BinOpNode node);
void if_code(IfNode if_node);

Memory* find_memory(char* id);
int new_reg();
int new_label();