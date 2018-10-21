#ifndef TABLE_H
#define TABLE_H
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
  NAT_VECTOR,
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

typedef struct SymbolElement {
  bool isSeparator;
  Symbol* symbol;
  char* name;
  struct SymbolElement* next;
} SymbolElement;

typedef struct SymbolsTable {
  SymbolElement* head;
  Symbol* return_symbol;
  Symbol* dot_symbol;
} SymbolsTable;

SymbolsTable* createTable();
void delete_table(SymbolsTable* table);

void pushScope(SymbolsTable* table);
void popScope(SymbolsTable* table);
void addSymbol(SymbolsTable* table, char* name, Symbol* symbol);
void setReturn(SymbolsTable* table, Symbol* symbol);
void setDot(SymbolsTable* table, Symbol* symbol);
void clearDot(SymbolsTable* table);

Symbol* getSymbol(SymbolsTable* table, char* name);
Symbol* getSymbolCurrentScope(SymbolsTable* table, char* name);
// Se retorno não está definido, encerra execução
Symbol* getReturn(SymbolsTable* table);
Symbol* getDot(SymbolsTable* table);

void print_table(SymbolsTable* table);
void print_symbol(const char* name, Symbol* symbol);
int size_for_type(TypeNode* type, SymbolsTable* table);

#endif