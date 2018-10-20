#include "table.h"
#include <string.h>

SymbolsTable* createTable() {
  SymbolsTable* table = malloc(sizeof(SymbolsTable));
  table->head = NULL;
  table->return_symbol = NULL;
  return table;
}

void addSymbol(SymbolsTable* table, char* name, Symbol* symbol) {
  SymbolElement* element = malloc(sizeof(SymbolElement));
  element->isSeparator = false;
  element->symbol = symbol;
  element->name = strdup(name);
  element->next = table->head;
  table->head = element;
}

Symbol* getSymbol(SymbolsTable* table, char* name) {
  SymbolElement* element = table->head;
  while (element != NULL &&
    (element->isSeparator || strcmp(name, element->name) != 0))
    element = element->next;
  if (element == NULL) return NULL;
  else return element->symbol;
}

Symbol* getSymbolCurrentScope(SymbolsTable* table, char* name) {
  SymbolElement* element = table->head;
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

void setDot(SymbolsTable* table, Symbol* symbol) {
  table->dot_symbol = symbol;
}

void clearDot(SymbolsTable* table) {
  table->dot_symbol = NULL;
}

Symbol* getDot(SymbolsTable* table) {
  return table->dot_symbol;
}

void delete_symbol(Symbol* symbol) {
  free(symbol);
}

void delete_element(SymbolElement* element) {
  if (element->name != NULL) free(element->name);
  if (element->next != NULL) delete_element(element->next);
  if (element->symbol != NULL) delete_symbol(element->symbol);
  free(element);
}

void pushScope(SymbolsTable* table) {
  SymbolElement* element = malloc(sizeof(SymbolElement));
  element->isSeparator = true;
  element->symbol = NULL;
  element->name = NULL;
  element->next = table->head;
  table->head = element;
}

void popScope(SymbolsTable* table) {
    while (table->head != NULL) {
      SymbolElement* cur = table->head;
      bool shouldTerminate = cur->isSeparator;
      table->head = cur->next;
      cur->next = NULL;
      delete_element(cur);

      if (shouldTerminate) return;
    }
}

void print_table(SymbolsTable* table) {
  #ifndef _DEBUG
    return;
  #endif
    printf("\n  vvvvvvvvvvvvvvvvvvvvv\n");
    SymbolElement* element = table->head;
    while (element != NULL) {
      if (element->isSeparator) {
        printf("\n  return: %s\n", table->return_symbol ? type_to_str(table->return_symbol->type) : "void");
        printf("  ---------------------\n");
      } else {
        print_symbol(element->name, element->symbol);
      }
      element = element->next;
    }
    printf("  ^^^^^^^^^^^^^^^^^^^^^^\n\n");
}

void print_symbol(const char* name, Symbol* symbol) {
  int i = 0;
  switch(symbol->nature) {
    case NAT_CLASS:
      printf("  - class %s [%d] (%d, %d)\n", name, symbol->size, symbol->line, symbol->column);
      FieldNode* fields = symbol->fields;
      while (fields != NULL) {
        printf("    field %d: %s %s [%d]\n", i++, type_to_str(fields->type), fields->identifier, size_for_type(fields->type, NULL));
        fields = fields->next;
      }
      break;
    case NAT_FUNCTION:
      printf("  - function %s: %s (%d, %d)\n", name, type_to_str(symbol->type), symbol->line, symbol->column);
      ParamNode* params = symbol->params;
      while (params != NULL) {
        printf("    param %d: %s %s\n", i++, type_to_str(params->type), params->identifier);
        params = params->next;
      }
      break;
    default:
      printf("  - %s %s [%d] (%d, %d)\n", type_to_str(symbol->type), name, symbol->size, symbol->line, symbol->column);
      break;
  }  
}

int size_for_type(TypeNode* type, SymbolsTable* table) {
  switch (type->kind) {
    case INT_T: return 4;
    case FLOAT_T: return 8;
    case CHAR_T: return 1;
    case BOOL_T: return 1;
    case STRING_T: return 0;
    case CUSTOM_T: {
      Symbol* s = getSymbol(table, type->name);
      if (s == NULL) return -1;
      else return s->size; }
  }
  return -1;
}
