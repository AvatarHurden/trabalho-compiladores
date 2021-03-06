#include "semantic.h"

#include <string.h>

// Se essa função retornar NULL, significa que o tipo passado não foi declarado (ERR_UNDECLARED)
Symbol* makeSymbol(enum Nature nature, TypeNode* type, SymbolsTable* table) {
  Symbol* s = malloc(sizeof(Symbol));
  s->nature = nature;
  s->type = type;
  s->params = NULL;
  s->fields = NULL;
  if (type == NULL)
    s->size = 0;
  else {
    int size = size_for_type(type, table);
    if (size == -1) return NULL;
    s->size = size;
  }
  s->line = 0;
  s->column = 0;
  return s;
}

bool match(TypeNode* t1, TypeNode* t2) {
  if (t1->kind == CUSTOM_T && t2->kind == CUSTOM_T)
    return strcmp(t1->name, t2->name) == 0;
  else
    return t1->kind == t2->kind;
}

int infer(TypeNode left, TypeNode right) {
  if (left.kind == STRING_T ||
      left.kind == CHAR_T ||
      left.kind == CUSTOM_T ||
      right.kind == STRING_T ||
      right.kind == CHAR_T ||
      right.kind == CUSTOM_T)
      return -1;
  if (left.kind == FLOAT_T || right.kind == FLOAT_T)
    return FLOAT_T;
  if (left.kind == INT_T || right.kind == INT_T)
    return INT_T;
  if (left.kind == BOOL_T || right.kind == BOOL_T)
    return BOOL_T;
  return -1;
}

int convert(TypeNode expected, TypeNode actual) {
  if (match(&expected, &actual))
    return expected.kind;
  if (infer(expected, actual) != -1) {
    return expected.kind;
  }
  return -1;
}

int check_program(Node* node) {
  SymbolsTable* table = createTable();
  TypeNode t;
  int check = typecheck(node, table, &t);
  if (check != 0) {
    printf("Semantic error: %s\n", semantic_error_to_str(check));
  }
  delete_table(table);
  return check;
}

int typecheck_var(VariableNode* var, SymbolsTable* table, TypeNode* out) {
  Symbol* s = getSymbol(table, var->identifier);
  if (s == NULL) return ERR_UNDECLARED;
  if (s->nature == NAT_FUNCTION) return ERR_FUNCTION;
  if (s->nature == NAT_CLASS) return ERR_USER;
  // Significa que foi usada como vetor e não é vetor na declaração (só pode ser simples)
  if (var->index != NULL && s->nature != NAT_VECTOR) return ERR_VARIABLE;
  // Significa que foi usada como simples, mas declarada como vetor
  if (var->index == NULL && s->nature == NAT_VECTOR) return ERR_VECTOR;
  if (var->index != NULL) {
    TypeNode index;
    int check = typecheck(var->index, table, &index);
    if (check != 0) return check;

    TypeNode intNode;
    intNode.kind = INT_T;
    int final_type = convert(intNode, index);
    if (final_type == -1)
      return ERR_WRONG_TYPE;
    if (final_type != index.kind) {
      var->index->coerced_to = final_type;
    }
  }

  if (var->field != NULL) {
    if (s->type->kind != CUSTOM_T)
      return ERR_VARIABLE;
    char* type_name = s->type->name;
    Symbol* s = getSymbol(table, type_name);
    FieldNode* f = s->fields;
    FieldNode* valid = NULL;
    while (f != NULL) {
      if (strcmp(f->identifier, var->field) == 0)
        valid = f;
      f = f->next;
    }
    if (valid == NULL) return ERR_UNDECLARED;
    *out = *(valid->type);
  } else {
    *out = *(s->type);
  }
  return 0;
}

int typecheck(Node* node, SymbolsTable* table, TypeNode* out) {
  if (node == NULL)
    return 0;

  switch (node->type) {
    // Global declarations
    case GLOBAL_VAR_DECL: {
      GlobalVarNode decl = node->value->global_var_node;
      #ifdef _DEBUG
        printf("> Checking global var %s\n", decl.identifier);
      #endif
      if (getSymbol(table, decl.identifier) != NULL)
        return ERR_DECLARED;

      enum Nature kind;
      if (decl.array_size >= 0)
        kind = NAT_VECTOR;
      else
        kind = NAT_VARIABLE;
      Symbol* s = makeSymbol(kind, decl.type, table);
      if (s == NULL) return ERR_UNDECLARED;
      if (decl.array_size > 0)
        s->size = decl.array_size * s->size;
      s->line = node->line;
      s->column = node->column;
      addSymbol(table, decl.identifier, s);
      print_table(table);
      return typecheck(node->next, table, out);
    }
    case TYPE_DECL: {
      TypeDeclNode decl = node->value->type_decl_node;
      #ifdef _DEBUG
        printf("> Checking class %s\n", decl.identifier);
      #endif

      if (getSymbol(table, decl.identifier) != NULL)
        return ERR_DECLARED;

      Symbol* s = makeSymbol(NAT_CLASS, NULL, table);
      if (s == NULL) return ERR_UNDECLARED;
      s->fields = decl.field;

      int size = 0;
      FieldNode* f = decl.field;
      while (f != NULL) {
        size += size_for_type(f->type, table);
        f = f->next;
      }
      s->size = size;
      s->line = node->line;
      s->column = node->column;
      addSymbol(table, decl.identifier, s);

      print_table(table);
      return typecheck(node->next, table, out);
    }
    case FUNCTION_DECL: {
      FunctionDeclNode decl = node->value->function_decl_node;
      #ifdef _DEBUG
        printf("> Checking function %s\n", decl.identifier);
      #endif

      if (getSymbol(table, decl.identifier) != NULL)
        return ERR_DECLARED;

      Symbol* s = makeSymbol(NAT_FUNCTION, decl.type, table);
      if (s == NULL) return ERR_UNDECLARED;
      s->params = decl.param;
      s->line = node->line;
      s->column = node->column;
      setReturn(table, s);
      addSymbol(table, decl.identifier, s);

      pushScope(table);
      ParamNode* param = decl.param;
      while (param != NULL) {
        Symbol* s = makeSymbol(NAT_VARIABLE, param->type, table);
        if (s == NULL) return ERR_UNDECLARED;
        s->line = param->line;
        s->column = param->column;
        addSymbol(table, param->identifier, s);
        param = param->next;
      }
      print_table(table);
      int check = typecheck(decl.body, table, out);
      if (check != 0) return check;

      popScope(table);
      return typecheck(node->next, table, out); }

    case VAR_DECL: {
      LocalVarNode decl = node->value->local_var_node;
      #ifdef _DEBUG
        printf("> Checking declaration of %s\n", decl.identifier);
      #endif

      if (getSymbolCurrentScope(table, decl.identifier) != NULL)
        return ERR_DECLARED;

      int len = -1;
      if (decl.init != NULL) {
        TypeNode init_type;
        int check = typecheck(decl.init, table, &init_type);
        if (check != 0) return check;

        int final_type = convert(*decl.type, init_type);
        if (final_type == -1)
          return ERR_WRONG_TYPE;
        if (final_type != init_type.kind) {
          //printf("\n%s coerced to %s\n", type_to_str(&init_type), kind_to_str(final_type));
          decl.init->coerced_to = final_type;
        }

        if (init_type.kind == STRING_T)
          len = strlen(decl.init->value->string_node);
      }

      Symbol* s = makeSymbol(NAT_VARIABLE, decl.type, table);
      if (s == NULL) return ERR_UNDECLARED;
      if (len > - 1) s->size = len;
      s->line = node->line;
      s->column = node->column;
      addSymbol(table, decl.identifier, s);
      print_table(table);
      return 0;
    }
    case INT:
      out->kind = INT_T;
      return 0;
    case FLOAT:
      out->kind = FLOAT_T;
      return 0;
    case BOOL:
      out->kind = BOOL_T;
      return 0;
    case CHAR:
      out->kind = CHAR_T;
      return 0;
    case STRING:
      out->kind = STRING_T;
      return 0;
    case VARIABLE: {
      VariableNode var = node->value->var_node;
      return typecheck_var(&var, table, out);
    }
    case DOT: {
      Symbol* s = getDot(table);
      if (s == NULL) return ERR_UNDECLARED;
      *out = *(s->type);
      return 0;
    }
    case BIN_OP: {
      BinOpNode bin = node->value->bin_op_node;

      TypeNode left_type;
      int left = typecheck(bin.left, table, &left_type);
      if (left != 0) return left;

      TypeNode right_type;
      if (bin.type != BASH_PIPE && bin.type != FORWARD_PIPE) {
        int right = typecheck(bin.right, table, &right_type);
        if (right != 0) return right;
      }

      switch (bin.type) {
        case ADD:
        case SUBTRACT:
        case MULTIPLY:
        case DIVIDE:
        case MODULO:
        case POW: {
          // Only numerical types are accepted
          int kind = infer(left_type, right_type);
          if (kind == -1) return ERR_WRONG_TYPE;
          out->kind = kind;
          return 0; }
        case GREATER:
        case LESS_THAN:
        case GREATER_EQUAL:
        case LESS_EQUAL:
        case EQUAL:
        case NOT_EQUAL:
          out->kind = BOOL_T;
          // Check for numerical inference
          int final_type = convert(left_type, right_type);
          if (final_type == -1)
            return ERR_WRONG_TYPE;
          if (final_type != left_type.kind) {
            //printf("\n%s coerced to %s\n", type_to_str(&left_type), kind_to_str(final_type));
            bin.left->coerced_to = final_type;
          }
          return 0;
        case AND:
        case OR:
          if (left_type.kind == BOOL_T && right_type.kind == BOOL_T) {
            out->kind = BOOL_T;
            return 0;
          }
          return ERR_WRONG_TYPE;
        case BIT_AND:
        case BIT_OR:
          out->kind = BOOL_T;
          // Check for numerical inference
          if (convert(left_type, right_type) != -1)
            return 0;
          return ERR_WRONG_TYPE;
        case BASH_PIPE:
        case FORWARD_PIPE: {
          Symbol* s = makeSymbol(NAT_VARIABLE, &left_type, table);
          setDot(table, s);

          int right = typecheck(bin.right, table, &right_type);
          if (right != 0) return right;

          clearDot(table);

          *out = right_type;

          return 0;
        }
      }

    }
    case UN_OP: {
      UnOpNode un = node->value->un_op_node;

      TypeNode value_type;
      int type = typecheck(un.value, table, &value_type);
      if (type != 0) return type;

      TypeNode bool_node;
      bool_node.kind = BOOL_T;

      switch (un.type) {
        case NOT: {
          int final_type = convert(bool_node, value_type);
          if (final_type == -1)
            return final_type;
          if (final_type != value_type.kind) {
            //printf("\n%s coerced to %s\n", type_to_str(&value_type), kind_to_str(final_type));
            node->coerced_to = final_type;
          }
          out->kind = final_type;
          return 0; }
        case MINUS:
        case PLUS: {
          int kind = infer(bool_node, value_type);
          if (kind == -1) return ERR_WRONG_TYPE;
          out->kind = kind;
          return 0; }
        case ADDRESS:
        case VALUE:
          return ERR_WRONG_TYPE;
        case EVAL_BOOL: {
          int kind = convert(bool_node, value_type);
          if (kind == -1) return ERR_WRONG_TYPE;
          out->kind = kind;
          return 0; }
        case HASH:
          return ERR_WRONG_TYPE;
      }
    }
    case TERN_OP: {
      TernOpNode tern = node->value->tern_op_node;

      TypeNode cond_type;
      int type = typecheck(tern.cond, table, &cond_type);
      if (type != 0) return type;

      TypeNode bool_node;
      bool_node.kind = BOOL_T;
      int kind = convert(bool_node, cond_type);
      if (kind == -1) return ERR_WRONG_TYPE;

      TypeNode exp1_type;
      type = typecheck(tern.exp1, table, &exp1_type);
      if (type != 0) return type;

      TypeNode exp2_type;
      type = typecheck(tern.exp2, table, &exp2_type);
      if (type != 0) return type;

      int ret_kind = convert(exp1_type, exp2_type);
      if (ret_kind == -1) return ERR_WRONG_TYPE;

      out->kind = ret_kind;
      return 0;
    }
    
    
    case ATTR: {
      AttrNode attr = node->value->attr_node;

      TypeNode var_type;
      int check = typecheck_var(attr.var, table, &var_type);
      if (check != 0) return check;

      TypeNode value_type;
      check = typecheck(attr.value, table, &value_type);
      if (check != 0) return check;

      int final_type = convert(var_type, value_type);
      if (final_type == -1)
        return ERR_WRONG_TYPE;
      if (final_type != value_type.kind) {
        //printf("\n%s coerced to %s\n", type_to_str(&value_type), kind_to_str(final_type));
        node->coerced_to = final_type;
      }

      if (value_type.kind == STRING_T && attr.value->type == STRING) {
        int len = strlen(attr.value->value->string_node);
        Symbol* s = getSymbol(table, attr.var->identifier);
        if (s != NULL && s->size == 0)
          s->size = len;
      }
      *out = var_type;
      return 0;
    }
    case SHIFT_L:
    case SHIFT_R: {
      AttrNode attr = node->value->attr_node;

      TypeNode var_type;
      int check = typecheck_var(attr.var, table, &var_type);
      if (check != 0) return check;

      TypeNode value_type;
      check = typecheck(attr.value, table, &value_type);
      if (check != 0) return check;

      TypeNode int_type;
      int_type.kind = INT_T;
      if (convert(int_type, value_type) == -1 ||
          convert(int_type, var_type) == -1)
        return ERR_WRONG_TYPE;
      *out = var_type;
      return 0;
    }
    case FUNCTION_CALL: {
      FunctionCallNode call = node->value->function_call_node;
      Symbol* s = getSymbol(table, call.identifier);
      if (s == NULL) return ERR_UNDECLARED;
      if (s->nature == NAT_VARIABLE) return ERR_VARIABLE;
      if (s->nature == NAT_CLASS) return ERR_USER;
      if (s->nature == NAT_VECTOR) return ERR_VECTOR;

      Node* arg = call.arguments;
      ParamNode* param = s->params;
      while (arg != NULL && param != NULL) {
        TypeNode arg_type;
        int check = typecheck(arg, table, &arg_type);
        if (check != 0) return check;
        if (convert(*param->type, arg_type) == -1)
          return ERR_WRONG_TYPE_ARGS;
        arg = arg->next;
        param = param->next;
      }
      if (arg != NULL) return ERR_EXCESS_ARGS;
      if (param != NULL) return ERR_MISSING_ARGS;
      out->kind = s->type->kind;
      return 0;
    }
    case RETURN: {
      ListNode ret = node->value->return_node;

      TypeNode value_type;
      int check = typecheck(ret.value, table, &value_type);
      if (check != 0) return check;

      Symbol* expected_return = getReturn(table);
      int kind = convert(*expected_return->type, value_type);
      if (kind != -1) {
        out->kind = kind;
        return 0;
      } else
      return ERR_WRONG_PAR_RETURN;
    }
    case INPUT: {
      ListNode input = node->value->input_node;

      if (input.value->type != VARIABLE) return ERR_WRONG_PAR_INPUT;

      TypeNode t;
      int check = typecheck(input.value, table, &t);
      if (check != 0) return check;

      return 0;
    }
    case OUTPUT: {
      ListNode output = node->value->output_node;

      Node* value = output.value;
      while (value != NULL) {
        // Literal de string
        if (value->type == STRING)
          value = value->next;
        else {
          // Verifica se pode virar uma expressão numérica
          TypeNode t;
          int check = typecheck(value, table, &t);
          if (check != 0) return check;
          TypeNode int_type;
          int_type.kind = INT_T;
          if (convert(int_type, t) == -1) return ERR_WRONG_PAR_OUTPUT;
          value = value->next;
        }
      }
      return 0;
    }
    case BREAK:
    case CONTINUE:
    case CASE:
      return 0;
    case BLOCK: {
      ListNode block = node->value->block_node;

      Node* value = block.value;
      while (value != NULL) {
        int check = typecheck(value, table, out);
        if (check != 0) return check;
        value = value->next;
      }
      return 0;
    }
    case IF: {
      IfNode iff = node->value->if_node;

      TypeNode cond_type;
      int check = typecheck(iff.cond, table, &cond_type);
      if (check != 0) return check;

      TypeNode b;
      b.kind = BOOL_T;

      int final_type = convert(b, cond_type);
      if (final_type == -1)
        return ERR_WRONG_TYPE;
      if (final_type != cond_type.kind) {
        //printf("\n%s coerced to %s\n", type_to_str(&cond_type), kind_to_str(final_type));
        node->coerced_to = final_type;
      }

      TypeNode then_type;
      check = typecheck(iff.then, table, &then_type);
      if (check != 0) return check;

      if (iff.else_node != NULL) {
        TypeNode else_type;
        check = typecheck(iff.else_node, table, &else_type);
        if (check != 0) return check;
      }
      return 0;
    }
    case WHILE: {
      WhileNode whilee = node->value->while_node;

      TypeNode cond_type;
      int check = typecheck(whilee.cond, table, &cond_type);
      if (check != 0) return check;

      TypeNode b;
      b.kind = BOOL_T;
      if (convert(b, cond_type) == -1) return ERR_WRONG_TYPE;

      TypeNode body_type;
      check = typecheck(whilee.body, table, &body_type);
      if (check != 0) return check;

      return 0;
    }
    case DO_WHILE: {
      WhileNode whilee = node->value->do_while_node;

      TypeNode cond_type;
      int check = typecheck(whilee.cond, table, &cond_type);
      if (check != 0) return check;

      TypeNode b;
      b.kind = BOOL_T;
      if (convert(b, cond_type) == -1) return ERR_WRONG_TYPE;

      TypeNode body_type;
      check = typecheck(whilee.body, table, &body_type);
      if (check != 0) return check;

      return 0;
    }
    case SWITCH: {
      SwitchNode switchh = node->value->switch_node;

      TypeNode exp_type;
      int check = typecheck(switchh.expression, table, &exp_type);
      if (check != 0) return check;

      TypeNode b;
      b.kind = INT_T;
      if (convert(b, exp_type) == -1) return ERR_WRONG_TYPE;

      TypeNode body_type;
      check = typecheck(switchh.body, table, &body_type);
      if (check != 0) return check;

      return 0;
    }
    case FOR: {
      ForNode forr = node->value->for_node;

      Node* init = forr.initializers;
      while (init != NULL) {
        TypeNode init_type;
        int check = typecheck(init, table, &init_type);
        if (check != 0) return check;
        init = init->next;
      }

      TypeNode exps_type;
      int check = typecheck(forr.expressions, table, &exps_type);
      if (check != 0) return check;

      Node* command = forr.commands;
      while (command != NULL) {
        TypeNode command_type;
        int check = typecheck(command, table, &command_type);
        if (check != 0) return check;
        command = command->next;
      }

      TypeNode body_type;
      check = typecheck(forr.body, table, &body_type);
      if (check != 0) return check;

      return 0;
    }
    case FOR_EACH: {
      ForEachNode for_each = node->value->for_each_node;

      Node* expr = for_each.expression;
      TypeNode common_type;
      bool isFirst = true;
      while (expr != NULL) {
        TypeNode exp_type;
        int check = typecheck(expr, table, &exp_type);
        if (check != 0) return check;
        if (isFirst)
          common_type = exp_type;
        isFirst = false;
        if (convert(common_type, exp_type) == -1) return ERR_WRONG_TYPE;
        expr = expr->next;
      }

      Symbol* s = makeSymbol(NAT_VARIABLE, &common_type, table);
      pushScope(table);
      addSymbol(table, for_each.id, s);

      TypeNode body_type;
      int check = typecheck(for_each.body, table, &body_type);
      if (check != 0) return check;

      popScope(table);

      return 0;
    }
  }

  return 0;
}

const char* semantic_error_to_str(int e) {
  switch(e) {
    case ERR_UNDECLARED: return "Identifier not declared";
    case ERR_DECLARED: return "Identifier already declared";
    case ERR_VARIABLE: return "Identifier should be used as variable";
    case ERR_VECTOR: return "Identifier should be used as vector";
    case ERR_FUNCTION: return "Identifier should be used as function";
    case ERR_USER: return "Identifier should be used as custom type";
    case ERR_WRONG_TYPE: return "Incompatible types";
    case ERR_STRING_TO_X: return "Impossible convertion of string variable";
    case ERR_CHAR_TO_X: return "Impossible convertion of char variable";
    case ERR_USER_TO_X: return "Impossible convertion of custom type variable";
    case ERR_MISSING_ARGS: return "Missing arguments";
    case ERR_EXCESS_ARGS: return "Arguments in excess";
    case ERR_WRONG_TYPE_ARGS: return "Wrong type arguments";
    case ERR_WRONG_PAR_INPUT: return "Input parameter should be an identifier";
    case ERR_WRONG_PAR_OUTPUT: return "Output parameter should be string or expression";
    case ERR_WRONG_PAR_RETURN: return "Wrong return type";
  }
  return "Unknown error";
}
