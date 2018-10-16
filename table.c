#include "table.h"
#include <string.h>

SymbolsTable* createTable() {
  SymbolsTable* table = malloc(sizeof(SymbolsTable));
  table->head = NULL;
  table->return_symbol = NULL;
  return table;
}

void addSymbol(SymbolsTable* table, char* name, Symbol* symbol) {
  struct SymbolElement* element = malloc(sizeof(struct SymbolElement));
  element->isSeparator = false;
  element->symbol = symbol;
  element->name = strdup(name);
  element->next = table->head;
  table->head = element;
}

Symbol* getSymbol(SymbolsTable* table, char* name) {
  struct SymbolElement* element = table->head;
  while (element != NULL &&
    (element->isSeparator || strcmp(name, element->name) != 0))
    element = element->next;
  if (element == NULL) return NULL;
  else return element->symbol;
}

Symbol* getSymbolCurrentScope(SymbolsTable* table, char* name) {
  struct SymbolElement* element = table->head;
  while (element != NULL &&
    (!element->isSeparator && strcmp(name, element->name) != 0))
    element = element->next;
  if (element == NULL) return NULL;
  else return element->symbol;
}

void setReturn(SymbolsTable* table, Symbol* symbol) {
  table->return_symbol = symbol;
}

// Se retorno não está definido, encerra execução
Symbol* getReturn(SymbolsTable* table) {
  if (table->return_symbol == NULL) {
    printf("Return statement without a defined return type\n");
    exit(-1);
  }
  return table->return_symbol;
}

void delete_symbol(Symbol* symbol) {
  free(symbol);
}

void delete_element(struct SymbolElement* element) {
  if (element->name != NULL) free(element->name);
  if (element->next != NULL) delete_element(element->next);
  if (element->symbol != NULL) delete_symbol(element->symbol);
  free(element);
}

void pushScope(SymbolsTable* table) {
  struct SymbolElement* element = malloc(sizeof(struct SymbolElement));
  element->isSeparator = true;
  element->symbol = NULL;
  element->name = NULL;
  element->next = table->head;
  table->head = element;
}

void popScope(SymbolsTable* table) {
    while (table->head != NULL) {
      struct SymbolElement* cur = table->head;
      bool shouldTerminate = cur->isSeparator;
      table->head = cur->next;
      cur->next = NULL;
      delete_element(cur);

      if (shouldTerminate) return;
    }
}
