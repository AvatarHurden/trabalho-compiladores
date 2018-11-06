#include "iloc.h"

#include "string.h"

int reg_counter = 0;
int label_counter = 0;
int global_offset = 0;
int local_offset = 0;
Memory* global_memory = NULL;

void generate_code(Node* node) {
    if (node == NULL) {
        return;
    }
    switch (node->type) {
    case GLOBAL_VAR_DECL:
        global_var_code(node->value->global_var_node);
        break;
    case FUNCTION_DECL:
        generate_code(node->value->function_decl_node.body);
        break;
    case BLOCK:
        generate_code(node->value->block_node.value);
        break;
    case VAR_DECL:
        local_var_code(node->value->local_var_node);
        break;
    case ATTR:
        attr_code(node->value->attr_node);
        break;
    case IF:
        if_code(node->value->if_node);
        break;
    default:
        expression_code(node);
        break;
    }

    generate_code(node->next);
}

void expression_code(Node* node) {
    switch (node->type) {
    case INT:
        int_code(node->value->int_node);
        break;
    case VARIABLE:
        var_access_code(node->value->var_node);
        break;
    case BIN_OP:
        bin_op_code(node->value->bin_op_node);
        break;
    default:
        break;
    }
}

void global_var_code(GlobalVarNode var_node) {
    Memory* m = (Memory*) malloc(sizeof(Memory));
    m->id = var_node.identifier;
    m->base_reg = "rbss";
    m->offset = global_offset;
    m->next = global_memory;
    global_memory = m;
    global_offset += 4;
}

void local_var_code(LocalVarNode var_node) {
    Memory* mem = (Memory*) malloc(sizeof(Memory));
    mem->id = var_node.identifier;
    mem->base_reg = "rfp";
    mem->offset = local_offset;
    mem->next = global_memory;
    global_memory = mem;
    local_offset += 4;

    if (var_node.init) {
        generate_code(var_node.init);
        printf("storeAI r%d => rfp, %d", reg_counter, mem->offset);
        printf(" // %s = r%d\n", var_node.identifier, reg_counter);
    }
}

void attr_code(AttrNode attr_node) {
    generate_code(attr_node.value);
    Memory* mem = find_memory(attr_node.var->identifier);
    printf("storeAI r%d => %s, %d", reg_counter, mem->base_reg, mem->offset);
    printf(" // %s = r%d\n", attr_node.var->identifier, reg_counter);
}

void int_code(int int_node) {
    printf("loadI %d => r%d\n", int_node, new_reg());
} 

void var_access_code(VariableNode var_node) {
    Memory* mem = find_memory(var_node.identifier);
    printf("loadAI %s, %d => r%d", mem->base_reg, mem->offset, new_reg());
    printf(" // r%d = %s\n", reg_counter, var_node.identifier);
}

void bin_op_code(BinOpNode node) {
    switch(node.type) {
    case AND:
        printf("// AND\n");
        expression_code(node.left);
        int result_reg = reg_counter;
        int zero_reg = new_reg();
        int cmp_reg = new_reg();
        printf("loadI 0 => r%d // LOAD ZERO (AND)\n", zero_reg);
        printf("cmp_NE r%d, r%d -> r%d", result_reg, zero_reg, cmp_reg);
        printf(" // Check if left result (r%d) is zero\n", result_reg);
        int eval_right = new_label();
        int skip_right = new_label();
        printf("cbr r%d -> L%d, L%d", cmp_reg, eval_right, skip_right);
        printf(" // If it was zero, skip right eval (short circuit)\n");
        printf("L%d: nop\n", eval_right);
        expression_code(node.right);
        printf("i2i r%d => r%d", reg_counter, result_reg);
        printf (" // Move right eval to result reg (r%d)\n", result_reg);
        printf("L%d: nop\n", skip_right);
        printf("i2i r%d => r%d", result_reg, new_reg());
        printf(" // Move AND result to r%d\n", reg_counter);
        break;
    default:
        break;
    }
}

void if_code(IfNode if_node) {
    printf("// IF\n");
    expression_code(if_node.cond);
    int result_reg = reg_counter;
    int zero_reg = new_reg();
    int cmp_reg = new_reg();
    printf("loadI 0 => r%d // LOAD ZERO (IF)\n", zero_reg);
    printf("cmp_NE r%d, r%d -> r%d", result_reg, zero_reg, cmp_reg);
    printf(" // Check if result (r%d) is zero\n", result_reg);
    int then_label = new_label();
    int else_label = new_label();
    int endif_label = new_label();
    printf("cbr r%d -> L%d, L%d", cmp_reg, then_label, else_label);
    printf(" // If it was zero, goto else (L%d)\n", else_label);
    printf("L%d: nop // then\n", then_label);
    generate_code(if_node.then);
    printf("jumpI -> L%d // goto done\n", endif_label);
    printf("L%d: nop // else\n", else_label);
    generate_code(if_node.else_node);
    printf("L%d: nop\n// ENDIF\n", endif_label);
}

Memory* find_memory(char* id) {
    Memory* mem = global_memory;
    while (mem != NULL) {
        if (strcmp(mem->id, id) == 0) {
            break;
        }
        mem = mem->next;
    }
    return mem;
}

int new_reg() {
    reg_counter++;
    return reg_counter;
}

int new_label() {
    label_counter++;
    return label_counter;
}

