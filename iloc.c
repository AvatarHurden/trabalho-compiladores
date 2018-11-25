#include "iloc.h"

#include "string.h"

int reg_counter = 0;
int label_counter = 0;
int global_offset = 0;
int local_offset = 0;
Memory* global_memory = NULL;

void init_iloc() {
    printf("loadI 0 => rfp\n");
    printf("loadI 0 => rsp\n");
    printf("loadI 0 => rbss\n");
    printf("jumpI => Lmain\n");
}

void generate_code(Node* node) {
    if (node == NULL) {
        return;
    }
    switch (node->type) {
    case GLOBAL_VAR_DECL:
        global_var_code(node->value->global_var_node);
        break;
    case FUNCTION_DECL:
        function_decl_code(node->value->function_decl_node);
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
    case INT:
        int_code(node->value->int_node);
        break;
    case VARIABLE:
        var_access_code(node->value->var_node);
        break;
    case BIN_OP:
        bin_op_code(node->value->bin_op_node);
        break;
    case UN_OP:
        un_op_code(node->value->un_op_node);
        break;
    case IF:
        if_code(node->value->if_node);
        break;
    case WHILE:
        while_code(node->value->while_node);
        break;
    case DO_WHILE:
        do_while_code(node->value->do_while_node);
        break;
    case FUNCTION_CALL:
        function_call_code(node->value->function_call_node);
        break;
    case RETURN:
        return_code(node->value->return_node);
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
    printf("addI rsp, 4 => rsp");
    printf(" // Local variable %s\n", mem->id);

    if (var_node.init) {
        generate_code(var_node.init);
        printf("storeAI r%d => rfp, %d", reg_counter, mem->offset);
        printf(" // int %s = r%d\n", var_node.identifier, reg_counter);
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
    case OR:
        logic_expression(node);
        break;
    case GREATER:
    case LESS_THAN:
    case GREATER_EQUAL:
    case LESS_EQUAL:
    case EQUAL:
    case NOT_EQUAL:
        relational_expression(node);
        break;
    case ADD:
    case SUBTRACT:
    case MULTIPLY:
    case DIVIDE:
        arithmetic_expression(node);
        break;
    default:
        break;
    }
}

void un_op_code(UnOpNode node) {
    generate_code(node.value);
    if (node.type == NOT) {
        int label_true = new_label();
        int label_false = new_label();
        int label_end = new_label();
        printf("cbr r%d -> L%d, L%d // NOT\n", reg_counter, label_true, label_false);
        printf("L%d: cmp_NE r%d, r%d -> r%d\n", label_true, reg_counter, reg_counter, reg_counter);
        printf("jumpI -> L%d\n", label_end);
        printf("L%d: cmp_EQ r%d, r%d -> r%d\n", label_false, reg_counter, reg_counter, reg_counter);
        printf("L%d: // END NOT\n", label_end);
    } else if (node.type == MINUS) {
        int expression_reg = reg_counter;
        int result_reg = new_reg();
        printf("rsubI r%d, 0 => r%d", expression_reg, result_reg);
        printf(" // r%d = 0 - r%d\n", result_reg, expression_reg);
    }
}

void logic_expression(BinOpNode node) {
    char op[4];
    strcpy(op, node.type == AND ? "AND" : "OR");
    printf("// %s\n", op);
    generate_code(node.left);
    int result_reg = reg_counter;
    int eval_right = new_label();
    int skip_right = new_label();
    if (node.type == AND)
        printf("cbr r%d -> L%d, L%d", result_reg, eval_right, skip_right);
    else
        printf("cbr r%d -> L%d, L%d", result_reg, skip_right, eval_right);
    printf(" // Depending on result, skip right eval (short circuit)\n");
    printf("L%d: nop\n", eval_right);
    generate_code(node.right);
    printf("i2i r%d => r%d", reg_counter, result_reg);
    printf (" // Move right eval to result reg (r%d)\n", result_reg);
    printf("L%d: nop\n", skip_right);
    printf("i2i r%d => r%d", result_reg, new_reg());
    printf(" // Move %s result to r%d\n", op, reg_counter);
}

void relational_expression(BinOpNode node) {
    generate_code(node.left);
    int left_result = reg_counter;
    generate_code(node.right);
    int right_result = reg_counter;
    int cmp_result = new_reg();

    char op[3];
    switch(node.type) {
        case GREATER: strcpy(op, "GT"); break;
        case LESS_THAN: strcpy(op, "LT"); break;
        case GREATER_EQUAL: strcpy(op, "GE"); break;
        case LESS_EQUAL: strcpy(op, "LE"); break;
        case EQUAL: strcpy(op, "EQ"); break;
        case NOT_EQUAL: strcpy(op, "NE"); break;
        default: break;
    }
    printf("cmp_%s r%d, r%d -> r%d", op, left_result, right_result, cmp_result);
    printf(" // r%d = r%d %s r%d\n", cmp_result, left_result, op, right_result);
}

void arithmetic_expression(BinOpNode node) {
    generate_code(node.left);
    int left_result = reg_counter;
    generate_code(node.right);
    int right_result = reg_counter;
    int result_reg = new_reg();

    char op[5];
    switch(node.type) {
        case ADD: strcpy(op, "add"); break;
        case SUBTRACT: strcpy(op, "sub"); break;
        case MULTIPLY: strcpy(op, "mult"); break;
        case DIVIDE: strcpy(op, "div"); break;
        default: break;
    }

    printf("%s r%d, r%d => r%d", op, left_result, right_result, result_reg);
    printf(" // r%d = r%d %s r%d\n", result_reg, left_result, op, right_result);
}

void if_code(IfNode if_node) {
    printf("// IF\n");
    generate_code(if_node.cond);
    int result_reg = reg_counter;
    int then_label = new_label();
    int else_label = new_label();
    int endif_label = new_label();
    printf("cbr r%d -> L%d, L%d", result_reg, then_label, else_label);
    printf(" // If result (r%d) is false, goto else (L%d)\n", result_reg, else_label);
    printf("L%d: nop // THEN\n", then_label);
    generate_code(if_node.then);
    printf("jumpI -> L%d // goto ENDIF\n", endif_label);
    printf("L%d: nop // ELSE\n", else_label);
    generate_code(if_node.else_node);
    printf("L%d: nop\n// ENDIF\n", endif_label);
}

void while_code(WhileNode while_node) {
    printf("// WHILE\n");
    int test_label = new_label();
    printf("L%d: nop // TEST\n", test_label);
    generate_code(while_node.cond);
    int test_result = reg_counter;
    int enter_label = new_label();
    int leave_label = new_label();
    printf("cbr r%d -> L%d, L%d", test_result, enter_label, leave_label);
    printf(" // If test result (r%d) is false, leave while(L%d)\n", test_result, leave_label);
    printf("L%d: nop // ENTER WHILE\n", enter_label);
    generate_code(while_node.body);
    printf("jumpI -> L%d // goto TEST\n", test_label);
    printf("L%d: nop // LEAVE WHILE\n", leave_label);
}

void do_while_code(WhileNode do_while_node) {
    int enter_label = new_label();
    printf("L%d: // ENTER DO WHILE\n", enter_label);
    generate_code(do_while_node.body);
    printf("// TEST\n");
    generate_code(do_while_node.cond);
    int test_result = reg_counter;
    int leave_label = new_label();
    printf("cbr r%d -> L%d, L%d", test_result, enter_label, leave_label);
    printf(" // If test result (r%d) is true, enter do while(L%d)\n", test_result, enter_label);
    printf("L%d: // LEAVE DO WHILE\n", leave_label);
}

void function_decl_code(FunctionDeclNode function_decl_node) {
    printf("L%s:\n", function_decl_node.identifier);
    if (strcmp(function_decl_node.identifier, "main") == 0) {
        local_offset = 0;
        generate_code(function_decl_node.body);
        printf("halt\n");
        return;
    }
    int paramCount = 0;
    ParamNode* params = function_decl_node.param;
    while(params) {
        Memory* mem = (Memory*) malloc(sizeof(Memory));
        mem->id = params->identifier;
        mem->base_reg = "rfp";
        mem->offset = 4 * paramCount + 12;
        mem->next = global_memory;
        global_memory = mem;
        paramCount++;
        params = params->next;
    }
    local_offset = 20;
    printf("i2i rsp => rfp // New frame\n");
    printf("addI rsp, %d => rsp // Update stack top\n", 4 * paramCount + 16);
    generate_code(function_decl_node.body);
    int rsp_reg = new_reg();
    printf("loadAI rfp, 4 => r%d // Get rsp\n", rsp_reg);
    int rfp_reg = new_reg();
    printf("loadAI rfp, 8 => r%d // Get rfp\n", rfp_reg);
    int return_address_reg = new_reg();
    printf("loadAI rfp, %d => r%d // Get return address\n", 4 * paramCount + 12, return_address_reg);
    printf("i2i r%d => rsp // Restore rsp\n", rsp_reg);
    printf("i2i r%d => rfp // Restore rfp\n", rfp_reg);
    printf("jump => r%d // Jump to return address\n", return_address_reg);
}

void function_call_code(FunctionCallNode func_call_node) {
    printf("// Calling function %s\n", func_call_node.identifier);
    printf("storeAI rsp => rsp, 4 // Push Stack Pointer\n");
    printf("storeAI rfp => rsp, 8 // Push Frame Pointer\n");
    int argCount = 0;
    Node* args = func_call_node.arguments;
    while (args) {
        printf("// Evaluating arg %d of function %s\n", argCount, func_call_node.identifier);
        generate_code(args);
        printf("storeAI r%d => rsp, %d", reg_counter, 4 * argCount + 12);
        printf(" // Push arg %d of %s to the stack\n", argCount, func_call_node.identifier);
        argCount++;
        args = args->next;
    }
    int return_offset_reg = new_reg();
    printf("addI rpc, 3 => r%d", return_offset_reg);
    printf(" // Get return address\n");
    printf("storeAI r%d => rsp, %d", return_offset_reg, 4 * argCount + 12);
    printf(" // Save Return address\n");
    printf("jumpI => L%s\n", func_call_node.identifier);
    printf("loadAI rsp, 0 => r%d // Load return value\n", new_reg());
}

void return_code(ListNode return_node) {
    printf("// Evaluate return expression\n");
    generate_code(return_node.value);
    printf("storeAI r%d => rfp, 0 // Save return value\n", reg_counter);
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

