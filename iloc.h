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

int new_reg();
int new_label();