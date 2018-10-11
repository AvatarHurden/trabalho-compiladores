#include "semantic.h"

int typecheck(Node* node, SymbolsTable* table, TypeNode* out) {

  switch (node->type) {
    case INT:
      out->kind = INT_T;
      return 0;
  }

  return 0;
}
