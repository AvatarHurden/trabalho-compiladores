#include "tree.h"
#include <string.h>

// Private functions

Node* make_node(NodeType type) {
  Node* n = (Node*) malloc(sizeof(Node));
  n->type = type;
  n->value = malloc(sizeof(union NodeValue));
  n->next = NULL;
  return n;
}

void free_node(Node* node) {
  free(node->value);
  free(node);
}

// Deletion Functions

void libera(Node* node) {
  delete(node);
}

void delete_type(TypeNode* type) {
  if (type == NULL)
    return;
  if (type->name != NULL)
    free(type->name);
  free(type);
}

void delete_param(ParamNode* node) {
  if (node == NULL)
    return;
  delete_type(node->type);
  delete_param(node->next);
  free(node->identifier);
  free(node);
}

void delete_field(FieldNode* node) {
  if (node == NULL)
    return;
  delete_type(node->type);
  delete_field(node->next);
  free(node->identifier);
  free(node);
}

void delete_var(VariableNode* var) {
  free(var->identifier);
  delete(var->index);
  if (var->field != NULL)
    free(var->field);
}

void delete(Node* node) {
  if (node == NULL)
    return;
  // Type-specific internal frees
  switch (node->type) {
    case INT:
    case FLOAT:
    case BOOL:
    case CHAR:
      break;
    case STRING:
      free(node->value->string_node);
      break;
    case VARIABLE:
      free(node->value->var_node.identifier);
      delete(node->value->var_node.index);
      if (node->value->var_node.field != NULL)
        free(node->value->var_node.field);
      break;
    case BIN_OP:
      delete(node->value->bin_op_node.left);
      delete(node->value->bin_op_node.right);
      break;
    case UN_OP:
      delete(node->value->un_op_node.value);
      break;
    case TERN_OP:
      delete(node->value->tern_op_node.cond);
      delete(node->value->tern_op_node.exp1);
      delete(node->value->tern_op_node.exp2);
      break;
    case GLOBAL_VAR_DECL:
      delete_type(node->value->global_var_node.type);
      free(node->value->global_var_node.identifier);
      break;
    case TYPE_DECL:
      free(node->value->type_decl_node.identifier);
      delete_field(node->value->type_decl_node.field);
      break;
    case FUNCTION_DECL:
      delete_type(node->value->function_decl_node.type);
      free(node->value->function_decl_node.identifier);
      delete_param(node->value->function_decl_node.param);
      delete(node->value->function_decl_node.body);
      break;
    case VAR_DECL:
      delete_type(node->value->local_var_node.type);
      free(node->value->local_var_node.identifier);
      delete(node->value->local_var_node.init);
      break;
    case ATTR:
      delete_var(node->value->attr_node.var);
      delete(node->value->attr_node.value);
      break;
    case SHIFT_L:
      delete_var(node->value->attr_node.var);
      delete(node->value->shift_l_node.value);
      break;
    case SHIFT_R:
      delete_var(node->value->attr_node.var);
      delete(node->value->shift_r_node.value);
      break;
    case FUNCTION_CALL:
      free(node->value->function_call_node.identifier);
      delete(node->value->function_call_node.arguments);
      break;
    case DOT:
      break;
    case RETURN:
      delete(node->value->return_node.value);
      break;
    case BREAK:
    case CONTINUE:
    case CASE:
      break;
    case INPUT:
      delete(node->value->input_node.value);
      break;
    case OUTPUT:
      delete(node->value->output_node.value);
      break;
    case BLOCK:
      delete(node->value->block_node.value);
      break;
    case IF:
      delete(node->value->if_node.cond);
      delete(node->value->if_node.then);
      delete(node->value->if_node.else_node);
      break;
    case WHILE:
      delete(node->value->while_node.cond);
      delete(node->value->while_node.body);
      break;
    case DO_WHILE:
      delete(node->value->do_while_node.cond);
      delete(node->value->do_while_node.body);
      break;
    case SWITCH:
      delete(node->value->switch_node.expression);
      delete(node->value->switch_node.body);
      break;
    case FOR:
      delete(node->value->for_node.initializers);
      delete(node->value->for_node.expressions);
      delete(node->value->for_node.commands);
      delete(node->value->for_node.body);
      break;
    case FOR_EACH:
      delete(node->value->for_each_node.expression);
      delete(node->value->for_each_node.body);
      free(node->value->for_each_node.id);
      break;
  }
  // Free the actual node (and value)
  delete(node->next);
  free_node(node);
}

// Print Function

void indent(int n) {
  for (int i = 0; i < n; i++)
    putchar('\t');
}

void print_type(TypeNode* type) {
  switch (type->type) {
    case INT_T:
      printf("int");
      break;
    case FLOAT_T:
      printf("float");
      break;
    case CHAR_T:
      printf("char");
      break;
    case STRING_T:
      printf("string");
      break;
    case BOOL_T:
      printf("bool");
      break;
    case CUSTOM_T:
      printf("%s", type->name);
      break;
  }
}

void print_variable(VariableNode* var) {
  printf("%s", var->identifier);
  if (var->index != NULL) {
    printf("[");
    print(var->index);
    printf("]");
  }
  if (var->field != NULL)
    printf("$%s", var->field);
}

void print_offset(Node* node, int offset) {
  if (node == NULL)
    return;
  // Type-specific internal frees
  switch (node->type) {
    case INT:
      indent(offset);
      printf("%d", node->value->int_node);
      break;
    case FLOAT:
      indent(offset);
      printf("%f", node->value->float_node);
      break;
    case BOOL:
      indent(offset);
      if (node->value->bool_node)
        printf("true");
      else
        printf("false");
      break;
    case CHAR:
      indent(offset);
      printf("'%c'", node->value->char_node);
      break;
    case STRING:
      indent(offset);
      printf("\"%s\"", node->value->string_node);
      break;
    case VARIABLE:
      indent(offset);
      VariableNode var = node->value->var_node;
      printf("%s", var.identifier);
      if (var.index != NULL) {
        printf("[");
        print_offset(var.index, 0);
        printf("]");
      }
      if (var.field != NULL)
        printf("$%s", var.field);
      break;
    case BIN_OP:
      indent(offset);
      BinOpNode bin = node->value->bin_op_node;
      printf("(");
      print_offset(bin.left, 0);

      switch (bin.type) {
        case ADD:
          printf(" + ");
          break;
        case SUBTRACT:
          printf(" - ");
          break;
        case MULTIPLY:
          printf(" * ");
          break;
        case DIVIDE:
          printf(" / ");
          break;
        case MODULO:
          printf(" %% ");
          break;
        case POW:
          printf(" ^ ");
          break;
        case GREATER:
          printf(" > ");
          break;
        case LESS_THAN:
          printf(" < ");
          break;
        case GREATER_EQUAL:
          printf(" >= ");
          break;
        case LESS_EQUAL:
          printf(" <= ");
          break;
        case EQUAL:
          printf(" == ");
          break;
        case NOT_EQUAL:
          printf(" != ");
          break;
        case AND:
          printf(" && ");
          break;
        case OR:
          printf(" || ");
          break;
        case BIT_AND:
          printf(" & ");
          break;
        case BIT_OR:
          printf(" | ");
          break;
        case BASH_PIPE:
          printf(" %%|%% ");
          break;
        case FORWARD_PIPE:
          printf(" %%>%% ");
          break;
      }

      print_offset(bin.right, 0);
      printf(")");
      break;
    case UN_OP:
      indent(offset);

      UnOpNode un = node->value->un_op_node;
      printf("(");

      switch (un.type) {
        case NOT:
          printf("!");
          break;
        case MINUS:
          printf("-");
          break;
        case PLUS:
          printf("+");
          break;
        case ADDRESS:
          printf("&");
          break;
        case VALUE:
          printf("*");
          break;
        case EVAL_BOOL:
          printf("?");
          break;
        case HASH:
          printf("#");
          break;
      }

      print_offset(un.value, 0);
      printf(")");
      break;
    case TERN_OP:
      indent(offset);
      TernOpNode tern = node->value->tern_op_node;
      printf("(");
      print_offset(tern.cond, 0);
      printf(" ? ");
      print_offset(tern.exp1, 0);
      printf(" : ");
      print_offset(tern.exp2, 0);
      printf(")");
      break;
    case TYPE_DECL:
      indent(offset);
      TypeDeclNode decl = node->value->type_decl_node;
      printf("class %s [\n", decl.identifier);

      FieldNode* f = decl.field;
      while (f != NULL) {
        indent(offset+1);
        switch (f->scope) {
          case PRIVATE:
            printf("private ");
            break;
          case PUBLIC:
            printf("public ");
            break;
          case PROTECTED:
            printf("protected ");
            break;
          case NO_SCOPE:
            break;
        }
        print_type(f->type);
        printf(" %s%s\n", f->identifier, f->next == NULL ? "" : " :");
        f = f->next;
      }

      indent(offset);
      printf("];");
      break;
    case GLOBAL_VAR_DECL:
      indent(offset);
      GlobalVarNode var_decl = node->value->global_var_node;
      printf("%s", var_decl.identifier);
      if (var_decl.array_size >= 0)
        printf("[%d]", var_decl.array_size);
      if (var_decl.is_static)
        printf(" static");
      printf(" ");
      print_type(var_decl.type);
      printf(";");
      break;
    case FUNCTION_DECL:
      indent(offset);
      FunctionDeclNode func_decl = node->value->function_decl_node;
      if (func_decl.is_static)
        printf("static ");
      print_type(func_decl.type);
      printf(" %s(", func_decl.identifier);

      ParamNode* param = func_decl.param;
      while (param != NULL) {
        if (param->is_const)
          printf("const ");
          print_type(param->type);
          printf(" %s", param->identifier);
          if (param->next != NULL)
            printf(", ");
          param = param->next;
      }
      printf(") ");
      print_offset(func_decl.body, 0);
      break;
    case VAR_DECL:
      indent(offset);
      LocalVarNode local_var_decl = node->value->local_var_node;
      if (local_var_decl.is_static)
        printf("static ");
      if (local_var_decl.is_const)
        printf("const ");
      print_type(local_var_decl.type);
      printf(" %s", local_var_decl.identifier);
      if (local_var_decl.init != NULL) {
        printf(" <= ");
        print_offset(local_var_decl.init, 0);
      }
      break;
    case ATTR:
      indent(offset);
      AttrNode attr = node->value->attr_node;
      print_variable(attr.var);
      printf(" = ");
      print_offset(attr.value, 0);
      break;
    case SHIFT_L:
      indent(offset);
      AttrNode shift_l = node->value->shift_l_node;
      print_variable(shift_l.var);
      printf(" << ");
      print_offset(shift_l.value, 0);
      break;
    case SHIFT_R:
      indent(offset);
      AttrNode shift_r = node->value->shift_r_node;
      print_variable(shift_r.var);
      printf(" >> ");
      print_offset(shift_r.value, 0);
      break;
    case FUNCTION_CALL: {
      indent(offset);
      FunctionCallNode func_call = node->value->function_call_node;
      printf("%s(", func_call.identifier);
      Node* value = func_call.arguments;
      while (value != NULL) {
        print_offset(value, 0);
        if (value->next)
          printf(", ");
        value = value->next;
      }
      printf(")");
      break; }
    case DOT: {
      indent(offset);
      printf(".");
      break; }
    case RETURN: {
      indent(offset);
      ListNode ret = node->value->return_node;
      printf("return ");
      print_offset(ret.value, 0);
      break; }
    case INPUT: {
      indent(offset);
      ListNode input = node->value->input_node;
      printf("input ");
      print_offset(input.value, 0);
      break; }
    case OUTPUT: {
      indent(offset);
      ListNode output = node->value->output_node;
      printf("output ");
      Node* value = output.value;
      while (value != NULL) {
        print_offset(value, 0);
        if (value->next)
          printf(", ");
        value = value->next;
      }
      break; }
    case BREAK: {
      indent(offset);
      printf("break");
      break; }
    case CONTINUE: {
      indent(offset);
      printf("continue");
      break; }
    case CASE: {
      indent(offset);
      printf("case %d:", node->value->case_node);
      break; }
    case BLOCK: {
      indent(offset);
      ListNode block = node->value->block_node;
      printf("{\n");
      Node* value = block.value;
      while (value != NULL) {
        print_offset(value, offset+1);
        if (value->type != CASE)
          printf(";\n");
        else
          printf("\n");
        value = value->next;
      }
      indent(offset);
      printf("}");
      break; }
    case IF: {
      indent(offset);
      IfNode iff = node->value->if_node;
      printf("if (");
      print_offset(iff.cond, 0);
      printf(")\n");
      print_offset(iff.then, offset);
      printf("\n");
      indent(offset);
      printf("else\n");
      print_offset(iff.else_node, offset);
      break; }
    case WHILE: {
      indent(offset);
      WhileNode whilee = node->value->while_node;
      printf("while (");
      print_offset(whilee.cond, 0);
      printf(")\n");
      print_offset(whilee.body, offset);
      break; }
    case DO_WHILE: {
      indent(offset);
      WhileNode whilee = node->value->do_while_node;
      printf("do\n");
      print_offset(whilee.body, offset);
      printf(" while (");
      print_offset(whilee.cond, 0);
      printf(")");
      break; }
    case SWITCH: {
      indent(offset);
      SwitchNode switchh = node->value->switch_node;
      printf("switch (");
      print_offset(switchh.expression, 0);
      printf(")\n");
      print_offset(switchh.body, offset);
      break; }
    case FOR: {
      indent(offset);
      ForNode forr = node->value->for_node;
      printf("for (");
      Node* init = forr.initializers;
      while (init != NULL) {
        print_offset(init, 0);
        if (init->next != NULL)
          printf(", ");
        init = init->next;
      }
      printf(" : ");
      print_offset(forr.expressions, 0);
      printf(" : ");
      Node* command = forr.commands;
      while (command != NULL) {
        print_offset(command, 0);
        if (command->next != NULL)
          printf(", ");
        command = command->next;
      }
      printf(")\n");
      print_offset(forr.body, offset);
      break; }
    case FOR_EACH: {
      indent(offset);
      ForEachNode for_each = node->value->for_each_node;
      printf("foreach (%s : ", for_each.id);
      Node* expr = for_each.expression;
      while (expr != NULL) {
        print_offset(expr, 0);
        if (expr->next != NULL)
          printf(", ");
        expr = expr->next;
      }
      printf(")\n");
      print_offset(for_each.body, offset);
      break; }
  }

  if (node->next == NULL)
    return;

  if (node->type == TYPE_DECL
    || node->type == GLOBAL_VAR_DECL
    || node->type == FUNCTION_DECL) {
    printf("\n\n");
    print_offset(node->next, offset);
  // } else {
  //   printf("\n");
  //   print_offset(node->next, offset);
  }

}

void print(Node* node) {
  print_offset(node, 0);
}

void descompila(Node* arvore) {
  print_offset(arvore, 0);
}

// Construction Functions

Node* make_int(int value) {
  Node* n = make_node(INT);
  n->value->int_node = value;
  return n;
}

Node* make_float(float value) {
  Node* n = make_node(FLOAT);
  n->value->float_node = value;
  return n;
}

Node* make_bool(bool value) {
  Node* n = make_node(BOOL);
  n->value->bool_node = value;
  return n;
}

Node* make_char(char value) {
  Node* n = make_node(CHAR);
  n->value->char_node = value;
  return n;
}

Node* make_string(char* value) {
  Node* n = make_node(STRING);
  n->value->string_node = strdup(value);
  return n;
}

Node* make_variable(char* id, Node* index, char* field) {
  Node* n = make_node(VARIABLE);
  n->value->var_node.identifier = strdup(id);
  n->value->var_node.index = index;
  if (field != NULL)
    n->value->var_node.field = strdup(field);
  else
    n->value->var_node.field = NULL;
  return n;
}

Node* make_bin_op(Node* left, BinOpType type, Node* right) {
  Node* n = make_node(BIN_OP);
  n->value->bin_op_node.left = left;
  n->value->bin_op_node.right = right;
  n->value->bin_op_node.type = type;
  return n;
}

Node* make_un_op(Node* value, UnOpType type) {
  Node* n = make_node(UN_OP);
  n->value->un_op_node.value = value;
  n->value->un_op_node.type = type;
  return n;
}

Node* make_tern_op(Node* cond, Node* exp1, Node* exp2) {
  Node* n = make_node(TERN_OP);
  n->value->tern_op_node.cond = cond;
  n->value->tern_op_node.exp1 = exp1;
  n->value->tern_op_node.exp2 = exp2;
  return n;
}

TypeNode* make_type(TypeType kind, char* name) {
  TypeNode* n = (TypeNode*) malloc(sizeof(TypeNode));
  n->type = kind;
  if (kind == CUSTOM_T)
    n->name = strdup(name);
  else
    n->name = NULL;
  return n;
}

Node* make_global_var(TypeNode* type, char* id, bool is_static, int array_size) {
  Node* n = make_node(GLOBAL_VAR_DECL);
  n->value->global_var_node.type = type;
  n->value->global_var_node.identifier = strdup(id);
  n->value->global_var_node.is_static = is_static;
  n->value->global_var_node.array_size = array_size;
  return n;
}

FieldNode* make_field(Scope scope, TypeNode* type, char* id) {
  FieldNode* n = (FieldNode*) malloc(sizeof(FieldNode));
  n->scope = scope;
  n->type = type;
  n->identifier = strdup(id);
  return n;
}

Node* make_type_decl(char* id, FieldNode* field) {
  Node* n = make_node(TYPE_DECL);
  n->value->type_decl_node.identifier = strdup(id);
  n->value->type_decl_node.field = field;
  return n;
}

ParamNode* make_param(bool is_const, TypeNode* type, char* id) {
  ParamNode* n = (ParamNode*) malloc(sizeof(ParamNode));
  n->is_const = is_const;
  n->type = type;
  n->identifier = strdup(id);
  return n;
}

Node* make_function_decl(TypeNode* type, char* id, bool is_static, ParamNode* param, Node* body) {
  Node* n = make_node(FUNCTION_DECL);
  n->value->function_decl_node.type = type;
  n->value->function_decl_node.identifier = strdup(id);
  n->value->function_decl_node.is_static = is_static;
  n->value->function_decl_node.param = param;
  n->value->function_decl_node.body = body;
  return n;
}

Node* make_local_var(TypeNode* type, char* id, bool is_static, bool is_const, Node* init) {
  Node* n = make_node(VAR_DECL);
  n->value->local_var_node.type = type;
  n->value->local_var_node.identifier = strdup(id);
  n->value->local_var_node.is_static = is_static;
  n->value->local_var_node.is_const = is_const;
  n->value->local_var_node.init = init;
  return n;
}

Node* make_attr(VariableNode* var, Node* value) {
  Node* n = make_node(ATTR);
  n->value->attr_node.var = var;
  n->value->attr_node.value = value;
  return n;
}

Node* make_shift_l(VariableNode* var, Node* value) {
  Node* n = make_node(SHIFT_L);
  n->value->shift_l_node.var = var;
  n->value->shift_l_node.value = value;
  return n;
}

Node* make_shift_r(VariableNode* var, Node* value) {
  Node* n = make_node(SHIFT_R);
  n->value->shift_r_node.var = var;
  n->value->shift_r_node.value = value;
  return n;
}

Node* make_function_call(char* id, Node* arguments) {
  Node* n = make_node(FUNCTION_CALL);
  n->value->function_call_node.identifier = strdup(id);
  n->value->function_call_node.arguments = arguments;
  return n;
}

Node* make_dot() {
  Node* n = (Node*) malloc(sizeof(Node));
  n->type = DOT;
  n->value = NULL;
  n->next = NULL;
  return n;
}

Node* make_return(Node* value) {
  Node* n = make_node(RETURN);
  n->value->return_node.value = value;
  return n;
}

Node* make_break() {
  Node* n = (Node*) malloc(sizeof(Node));
  n->type = BREAK;
  n->value = NULL;
  n->next = NULL;
  return n;
}

Node* make_continue() {
  Node* n = (Node*) malloc(sizeof(Node));
  n->type = CONTINUE;
  n->value = NULL;
  n->next = NULL;
  return n;
}

Node* make_case(int value) {
  Node* n = make_node(CASE);
  n->value->case_node = value;
  return n;
}

Node* make_input(Node* value) {
  Node* n = make_node(INPUT);
  n->value->input_node.value = value;
  return n;
}

Node* make_output(Node* value) {
  Node* n = make_node(OUTPUT);
  n->value->output_node.value = value;
  return n;
}

Node* make_block(Node* value) {
  Node* n = make_node(BLOCK);
  n->value->block_node.value = value;
  return n;
}

Node* make_if(Node* cond, Node* then, Node* else_node) {
  Node* n = make_node(IF);
  n->value->if_node.cond = cond;
  n->value->if_node.then = then;
  n->value->if_node.else_node = else_node;
  return n;
}

Node* make_for_each(char* id, Node* expression, Node* body) {
  Node* n = make_node(FOR_EACH);
  n->value->for_each_node.id = strdup(id);
  n->value->for_each_node.expression = expression;
  n->value->for_each_node.body = body;
  return n;
}

Node* make_for(Node* initializers, Node* expressions, Node* commands, Node* body) {
  Node* n = make_node(FOR);
  n->value->for_node.initializers = initializers;
  n->value->for_node.expressions = expressions;
  n->value->for_node.commands = commands;
  n->value->for_node.body = body;
  return n;
}

Node* make_while(Node* cond, Node* body) {
  Node* n = make_node(WHILE);
  n->value->while_node.cond = cond;
  n->value->while_node.body = body;
  return n;
}

Node* make_do_while(Node* cond, Node* body) {
  Node* n = make_node(DO_WHILE);
  n->value->do_while_node.cond = cond;
  n->value->do_while_node.body = body;
  return n;
}

Node* make_switch(Node* expression, Node* body) {
  Node* n = make_node(SWITCH);
  n->value->switch_node.expression = expression;
  n->value->switch_node.body = body;
  return n;
}
