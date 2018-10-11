#include "semantic.h"

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
  }

  return 0;
}
