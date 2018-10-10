#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "tree.h"

enum Nature {
  NAT_LITERAL_INT,
  NAT_LITERAL_FLOAT,
  NAT_LITERAL_CHAR,
  NAT_LITERAL_STRING,
  NAT_LITERAL_BOOL,
  NAT_VARIABLE,
  NAT_FUNCTION,
  NAT_CLASS
 };

typedef struct Symbol {
  int line, column;
  int size;
  enum Nature nature;
  TypeNode* type;
  ParamNode* params;
  FieldNode* fields;
} Symbol;

struct SymbolElement {
  bool isSeparator;
  Symbol* symbol;
  char* name;
};

typedef struct SymbolsTable {
  struct SymbolElement* head;
} SymbolsTable;

void pushScope(SymbolsTable* table);
void popScope(SymbolsTable* table);
void addSymbol(SymbolsTable* table, char* name, Symbol* symbol);
Symbol* getSymbol(SymbolsTable* table, char* name);
