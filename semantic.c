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
  int size = size_for_type(type, table);
  if (size == -1) return NULL;
  s->size = size;
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
      popScope(table);
      return check; }
    case RETURN: {
      ListNode ret = node->value->return_node;

      int check = typecheck(ret.value, table, out);
      if (check != 0) return check;

      Symbol* expected_return = getReturn(table);
      if (match(out, expected_return->type))
        return 0;
      else
        return ERR_WRONG_PAR_RETURN; }
    case BLOCK: {
      int check = typecheck(node->value->block_node.value, table, out);
      return check; }
  }

  return 0;
}
