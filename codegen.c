#include "codegen.h"

#include "string.h"
#include <stdio.h>

#define OFFSET(val) (val * 4)
#define RETURN_VALUE_OFFSET 0
#define PARAMS_OFFSET(param_count) (RETURN_VALUE_OFFSET + OFFSET(param_count))
#define RETURN_ADDRESS_OFFSET(param_count) (PARAMS_OFFSET(param_count) + 4)
#define RFP_OFFSET(param_count) (RETURN_ADDRESS_OFFSET(param_count) + 4)
#define STATIC_LINK_OFFSET(param_count) (RFP_OFFSET(param_count) + 4)
#define STATE_OFFSET(param_count, temp_count) (STATIC_LINK_OFFSET(param_count) + OFFSET(temp_count))
#define PARAM_OFFSET(index) (RETURN_VALUE_OFFSET + OFFSET(index) + 4)

int reg_counter = 0;
int temps_used = 0;
int label_counter = -1;
int global_offset = 0;
int local_var_count = 0;
int return_label;
Memory* global_memory = NULL;
FunctionInfo* global_function_info = NULL;

void init_iloc() {
    loadI(1024, RFP);
    loadI(1024, RSP);
    loadI(0, RBSS);
    jumpI(0)->comment = "Goto main";
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
        push_scope();
        function_decl_code(node->value->function_decl_node);
        pop_scope();
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
    case UN_OP:
        un_op_code(node->value->un_op_node);
        break;
    case FUNCTION_CALL:
        function_call_code(node->value->function_call_node);
    default:
        break;
    }
}

void global_var_code(GlobalVarNode var_node) {
    new_memory(var_node.identifier, RBSS, global_offset);
    global_offset += 4;
}

void local_var_code(LocalVarNode var_node) {
    new_memory(var_node.identifier, RFP, OFFSET(local_var_count));
    addI(RSP, 4, RSP)->comment = "Local variable";

    if (var_node.init) {
        expression_code(var_node.init);
        storeAI(reg_counter, RFP, OFFSET(local_var_count));
        add_comment("Local var initialization");
    }

    local_var_count++;
}

void attr_code(AttrNode attr_node) {
    expression_code(attr_node.value);
    Memory* mem = find_memory(attr_node.var->identifier);

    storeAI(reg_counter, mem->base_reg, mem->offset);
    add_comment(attr_node.var->identifier);
}

void int_code(int int_node) {
    loadI(int_node, new_reg());
} 

void var_access_code(VariableNode var_node) {
    Memory* mem = find_memory(var_node.identifier);
    loadAI(mem->base_reg, mem->offset, new_reg());
    add_comment(var_node.identifier);
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
    expression_code(node.value);
    if (node.type == NOT) {
        int label_true = new_label();
        int label_false = new_label();
        int label_end = new_label();
        cbr(reg_counter, label_true, label_false)->comment = "NOT";
        compare(NOT_EQUAL, reg_counter, reg_counter, reg_counter)->label = label_true;
        jumpI(label_end);
        compare(EQUAL, reg_counter, reg_counter, reg_counter)->label = label_false;
        add_comment("END NOT")->label = label_end;
    } else if (node.type == MINUS) {
        int expression_reg = reg_counter;
        int result_reg = new_reg();
        rsubI(expression_reg, 0, result_reg)->comment = "Negative";
    }
}

void logic_expression(BinOpNode node) {
    new_comment(node.type == AND ? "AND" : "OR");
    expression_code(node.left);
    int result_reg = reg_counter;
    int eval_right = new_label();
    int skip_right = new_label();
    if (node.type == AND)
        cbr(result_reg, eval_right, skip_right);
    else
        cbr(result_reg, skip_right, eval_right);
    add_comment("Depending on result, skip right eval (short circuit)");
    add_comment("Eval right")->label = eval_right;
    expression_code(node.right);
    i2i(reg_counter, result_reg);
    add_comment("Move right eval to result reg (r%d)\n");
    add_comment("Skip right")->label = skip_right;
    i2i(result_reg, new_reg());
    add_comment("End logical exp");
}

void relational_expression(BinOpNode node) {
    expression_code(node.left);
    int left_result = reg_counter;
    expression_code(node.right);
    int right_result = reg_counter;
    int cmp_result = new_reg();
    compare(node.type, left_result, right_result, cmp_result);
    add_comment("Relop");
}

void arithmetic_expression(BinOpNode node) {
    expression_code(node.left);
    int left_result = reg_counter;
    expression_code(node.right);
    int right_result = reg_counter;
    int result_reg = new_reg();
    math(node.type, left_result, right_result, result_reg);
    add_comment("Arithmetic exp");
}

void if_code(IfNode if_node) {
    new_comment("IF");
    expression_code(if_node.cond);
    int result_reg = reg_counter;
    int then_label = new_label();
    int else_label = new_label();
    int endif_label = new_label();
    cbr(result_reg, then_label, else_label);
    add_comment("If result is false, goto else");
    add_comment("THEN")->label = then_label;
    generate_code(if_node.then);
    jumpI(endif_label);
    add_comment("goto ENDIF");
    add_comment("ELSE")->label = else_label;
    generate_code(if_node.else_node);
    add_comment("ENDIF")->label = endif_label;
}

void while_code(WhileNode while_node) {
    new_comment("WHILE");
    int test_label = new_label();
    add_comment("TEST")->label = test_label;
    expression_code(while_node.cond);
    int test_result = reg_counter;
    int enter_label = new_label();
    int leave_label = new_label();
    cbr(test_result, enter_label, leave_label);
    add_comment("If test result is false, leave while");
    add_comment("ENTER WHILE")->label = enter_label;
    generate_code(while_node.body);
    jumpI(test_label)->comment = "goto TEST";
    add_comment("LEAVE WHILE")->label = leave_label;
}

void do_while_code(WhileNode do_while_node) {
    int enter_label = new_label();
    new_comment("ENTER DO WHILE")->label = enter_label;
    generate_code(do_while_node.body);
    new_comment("TEST");
    generate_code(do_while_node.cond);
    int test_result = reg_counter;
    int leave_label = new_label();
    cbr(test_result, enter_label, leave_label);
    add_comment("If test result is true, goto ENTER DO WHILE");
    new_comment("LEAVE DO WHILE")->label = leave_label;
}

void function_decl_code(FunctionDeclNode function_decl_node) {
    new_comment("New function");
    int function_label = new_label();
    new_function_info(function_decl_node.identifier, function_label);
    new_comment(function_decl_node.identifier)->label = function_label;
    temps_used = 0;
    int param_count = function_params_code(function_decl_node.param);
    
    if (strcmp(function_decl_node.identifier, "main") == 0) {
        local_var_count = 0;
        set_main_label(function_label);
        generate_code(function_decl_node.body);
        halt();
    } else {
        local_var_count = param_count + 4; // retval, retaddr, rfp, static
        int rfp_reg = new_reg();
        i2i(RFP, rfp_reg);
        add_comment("Get RFP (Dynamic link)");
        i2i(RSP, RFP);
        add_comment("Update RFP");
        // Save State
        storeAI(rfp_reg, RFP, RFP_OFFSET(param_count));
        add_comment("Save RFP (Dynamic Link)");
        int static_link_reg = new_reg();
        loadI(0, static_link_reg)->comment = "Get static link";
        storeAI(static_link_reg, RFP, STATIC_LINK_OFFSET(param_count));
        add_comment("Save static link");
        /*
        for(int i = 0; i < temps_used; i++) {
            storeAI(i, RFP, STATE_OFFSET(param_count, i));
            add_comment("Save state");
        }
        */
        addI(RSP, STATIC_LINK_OFFSET(param_count) + 4, RSP);
        add_comment("Update RSP");
        return_label = new_label();
        generate_code(function_decl_node.body);
        new_comment("Return label")->label = return_label;
        // Restore State
        int return_address_reg = new_reg();
        loadAI(RFP, RETURN_ADDRESS_OFFSET(param_count), return_address_reg);
        add_comment("Load return address");
        int restored_rfp_reg = new_reg();
        loadAI(RFP, RFP_OFFSET(param_count), restored_rfp_reg);
        add_comment("Load RFP (Dynamic link)");
        /*
        for (int i = 0; i < temps_used; i++) {
            loadAI(RFP, STATE_OFFSET(param_count, i), i);
            add_comment("Restore state");
        }
        */
        i2i(restored_rfp_reg, RFP);
        i2i(RFP, RSP);
        jump(return_address_reg);
    }
}

int function_params_code(ParamNode* params) {
    int param_count = 0;
    while(params) {
        new_memory(params->identifier, RFP, PARAMS_OFFSET(param_count) + 4);
        param_count++;
        params = params->next;
    }
    return param_count;
}

void function_call_code(FunctionCallNode func_call_node) {
    new_comment(func_call_node.identifier);
    FunctionInfo* info = find_function_info(func_call_node.identifier);
    int current_rsp = new_reg();
    i2i(RSP, current_rsp)->comment = "Get current RSP";
    addI(RSP, 4, RSP)->comment = "Push space for return value";
    function_args_code(func_call_node);
    int return_address_reg = new_reg();
    addI(RPC, 4, return_address_reg);
    add_comment("Get return address");
    storeAI(return_address_reg, RSP, 0);
    add_comment("Save return address");
    i2i(current_rsp, RSP);
    jumpI(info->label);
    loadAI(RSP, RETURN_VALUE_OFFSET, new_reg());
    add_comment("Load return value");
}

int function_args_code(FunctionCallNode function_call_node) {
    int arg_count = 0;
    Node* args = function_call_node.arguments;
    while (args) {
        new_comment("Evaluating arg");
        expression_code(args);
        storeAI(reg_counter, RSP, 0);
        addI(RSP, 4, RSP)->comment = "Push arg to the stack";
        args = args->next;
        arg_count++;
    }
    return arg_count;
}

int function_arg_count(Node* args) {
    int arg_count = 0;
    while (args) {
        arg_count++;
        args = args->next;
    }
    return arg_count;
}

void return_code(ListNode return_node) {
    new_comment("Evaluate return expression");
    expression_code(return_node.value);
    storeAI(reg_counter, RFP, RETURN_VALUE_OFFSET);
    add_comment("Save return value");
    jumpI(return_label);
}

void new_memory(char* id, SpecialRegister base_reg, int offset) {
    Memory* m = (Memory*) malloc(sizeof(Memory));
    m->id = id;
    m->base_reg = base_reg;
    m->offset = offset;
    m->next = global_memory;
    global_memory = m;
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

FunctionInfo* new_function_info(char* id, int label) {
    FunctionInfo* i = (FunctionInfo*) malloc(sizeof(FunctionInfo));
    i->id = id;
    i->label = label;
    i->next = global_function_info;
    global_function_info = i;
    return i;
}
FunctionInfo* find_function_info(char* id) {
    FunctionInfo* i = global_function_info;
    while (i != NULL) {
        if (strcmp(i->id, id) == 0) {
            break;
        }
        i = i->next;
    }
    return i;
}

void push_scope() {
    new_memory("-", 0, 0);
}

void pop_scope() {
    while (global_memory != NULL) {
        Memory* mem = global_memory;
        bool stop = mem->id[0] == '-';
        global_memory = mem->next;
        mem->next = NULL;
        free(mem);
        if (stop) return;
    }
}

int new_reg() {
    temps_used++;
    reg_counter++;
    return reg_counter;
}

int new_label() {
    label_counter++;
    return label_counter;
}