#include "semantic.h"
#include "tree.h"
#include <string.h>

bool match(TypeNode* t1, TypeNode* t2) {
  if (t1->kind == CUSTOM_T && t2->kind == CUSTOM_T)
    return strcmp(t1->name, t2->name) == 0;
  else
    return t1->kind == t2->kind;
}

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

int typecheck(Node* node, SymbolsTable* table, TypeNode* out) {

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
    case VARIABLE: {
      VariableNode var = node->value->var_node;
      Symbol* s = getSymbol(table, var.identifier);
      if (s == NULL) return ERR_UNDECLARED;
      *out = *(s->type);
      return 0; }
    case FUNCTION_DECL: {
      FunctionDeclNode decl = node->value->function_decl_node;
      Symbol* s = malloc(sizeof(Symbol*));
      s->nature = NAT_FUNCTION;
      s->type = decl.type;
      s->params = decl.param;
      s->fields = NULL;
      int size = size_for_type(decl.type, table);
      if (size == -1) return ERR_UNDECLARED;
      s->size = size;
      s->line = 1;
      s->column = 1;
      addSymbol(table, decl.identifier, s);
      pushScope(table);
      ParamNode* param = decl.param;
      while (param != NULL) {
        Symbol* s = malloc(sizeof(Symbol));
        s->nature = NAT_VARIABLE;
        s->type = param->type;
        s->params = NULL;
        s->fields = NULL;
        int size = size_for_type(param->type, table);
        if (size == -1) return ERR_UNDECLARED;
        s->size = 0;
        s->line = 1;
        s->column = 1;

        addSymbol(table, param->identifier, s);
        param = param->next;
      }
      TypeNode out2;
      int check = typecheck(decl.body, table, &out2);
      if (check != 0) return check;
      popScope(table);
      if (match(&out2, decl.type))
        return 0;
      else
        return ERR_WRONG_PAR_RETURN;
    }
    case RETURN: {
      ListNode ret = node->value->return_node;
      int check = typecheck(ret.value, table, out);
      if (check != 0) return check;
      return 0;
    }
    case BLOCK: {
      int check = typecheck(node->value->block_node.value, table, out);
      return check;
    }

  }

  return 0;
}
