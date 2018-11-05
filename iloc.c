#include "iloc.h"

int reg_counter = 0;
int label_counter = 0;
int global_offset = 0;
int local_offset = 0;
Memory* global_memory = NULL;

void generate_code(Node* node) {
    if (node == NULL) {
        return;
    }
    switch(node->type) {
        case GLOBAL_VAR_DECL:
        global_var_code(node->value->global_var_node);
        break;
        case FUNCTION_DECL:
        break;
        case VAR_DECL:
        local_var_code(node->value->local_var_node);
        break;
        default:
        break;
    }

    generate_code(node->next);
}

void global_var_code(GlobalVarNode var_node) {
    Memory* m = (Memory*) malloc(sizeof(Memory));
    m->id = var_node.identifier;
    m->base_reg = "rbss";
    m->offset = global_offset;
    m->next = global_memory;
    global_memory = m;
}

void local_var_code(LocalVarNode var_node) {
    Memory* m = (Memory*) malloc(sizeof(Memory));
    m->id = var_node.identifier;
    m->base_reg = "rfp";
    m->offset = local_offset;
    m->next = global_memory;
    global_memory = m;

    if (var_node.init) {
        generate_code(var_node.init);
        printf("storeAI r%d => rfp, %d\n", reg_counter, m->offset);
    }
}


int new_reg() {
    reg_counter++;
    return reg_counter;
}

int new_label() {
    label_counter++;
    return label_counter;
}

