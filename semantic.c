#include "semantic.h"
#include "tree.h"
#include <string.h>

int size_for_type(TypeNode* type, SymbolsTable* table) {
  switch (type->kind) {
    case INT_T: return 4;
    case FLOAT_T: return 8;
    case CHAR_T: return 1;
    case BOOL_T: return 1;
    case STRING_T: return -1;
    case CUSTOM_T: {
      Symbol* s = getSymbol(table, type->name);
      if (s == NULL) return -1;
      else return s->size; }
  }
}

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
  if (infer(expected, actual) != -1)
    return expected.kind;
  return -1;
}

int typecheck(Node* node, SymbolsTable* table, TypeNode* out) {
  if (node == NULL)
    return 0;

  switch (node->type) {
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
    case BIN_OP: {
      BinOpNode bin = node->value->bin_op_node;

      TypeNode left_type;
      int left = typecheck(bin.left, table, &left_type);
      if (left != 0) return left;

      TypeNode right_type;
      int right = typecheck(bin.right, table, &right_type);
      if (right != 0) return right;

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
          // Check for numerical inference
          if (infer(left_type, right_type) != -1) {
            out->kind = BOOL_T;
            return 0;
          }
          return ERR_WRONG_TYPE;
        case EQUAL:
        case NOT_EQUAL:
          out->kind = BOOL_T;
          // Check for exact matches
          if (match(&left_type, &right_type))
            return 0;
          // Check for numerical inference
          if (infer(left_type, right_type) != -1)
            return 0;
          return ERR_WRONG_TYPE;
        case AND:
        case OR:
          if (left_type.kind == BOOL_T && right_type.kind == BOOL_T) {
            out->kind = BOOL_T;
            return 0;
          }
          return ERR_WRONG_TYPE;
        case BIT_AND:
        case BIT_OR:
          return ERR_WRONG_TYPE;
        case BASH_PIPE:
        case FORWARD_PIPE:
          return ERR_WRONG_TYPE;
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
          int kind = convert(bool_node, value_type);
          if (kind == -1) return kind;
          out->kind = kind;
          return 0; }
        case MINUS:
        case PLUS: {
          int kind = infer(bool_node, value_type);
          if (kind == -1) return kind;
          out->kind = kind;
          return 0; }
        case ADDRESS:
        case VALUE:
          return ERR_WRONG_TYPE;
        case EVAL_BOOL: {
          int kind = convert(bool_node, value_type);
          if (kind == -1) return kind;
          out->kind = kind;
          return 0; }
        case HASH:
          return ERR_WRONG_TYPE;
      }
    }
    case VARIABLE: {
      VariableNode var = node->value->var_node;
      Symbol* s = getSymbol(table, var.identifier);
      if (s == NULL) return ERR_UNDECLARED;
      if (s->nature == NAT_FUNCTION) return ERR_FUNCTION;
      if (s->nature == NAT_CLASS) return ERR_USER;
      // Significa que foi usada como vetor e não é vetor na declaração (só pode ser simples)
      if (var.index != NULL && s->nature != NAT_VECTOR) return ERR_VARIABLE;
      // Significa que foi usada como simples, mas declarada como vetor
      if (var.index == NULL && s->nature == NAT_VECTOR) return ERR_VECTOR;
      *out = *(s->type);
      return 0; }
    case TYPE_DECL: {
      TypeDeclNode decl = node->value->type_decl_node;

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
      addSymbol(table, decl.identifier, s);

      return typecheck(node->next, table, out);
    }
    case GLOBAL_VAR_DECL: {
      GlobalVarNode decl = node->value->global_var_node;

      enum Nature kind;
      if (decl.array_size >= 0)
        kind = NAT_VECTOR;
      else
        kind = NAT_VARIABLE;
      Symbol* s = makeSymbol(kind, decl.type, table);
      if (s == NULL) return ERR_UNDECLARED;
      addSymbol(table, decl.identifier, s);

      return typecheck(node->next, table, out);
    }
    case FUNCTION_DECL: {
      FunctionDeclNode decl = node->value->function_decl_node;

      Symbol* s = makeSymbol(NAT_FUNCTION, decl.type, table);
      if (s == NULL) return ERR_UNDECLARED;
      s->params = decl.param;
      setReturn(table, s);
      addSymbol(table, decl.identifier, s);

      pushScope(table);
      ParamNode* param = decl.param;
      while (param != NULL) {
        Symbol* s = makeSymbol(NAT_VARIABLE, param->type, table);
        if (s == NULL) return ERR_UNDECLARED;
        addSymbol(table, param->identifier, s);
        param = param->next;
      }

      int check = typecheck(decl.body, table, out);
      if (check != 0) return check;

      popScope(table);
      return typecheck(node->next, table, out); }
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
        return ERR_WRONG_PAR_RETURN; }
    case BLOCK: {
      int check = typecheck(node->value->block_node.value, table, out);
      return check; }
  }

  return 0;
}
