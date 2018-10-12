/*
Função principal para realização da E3.

Este arquivo não pode ser modificado.
*/
#include <stdio.h>
#include "lex.yy.h"
#include "semantic.h"
#include "parser.tab.h" //arquivo gerado com bison -d parser.y

void *arvore = NULL;
void descompila (void *arvore);
void libera (void *arvore);

int main (int argc, char **argv)
{
  int ret = yyparse();
  descompila (arvore);
  SymbolsTable* table = createTable();
  TypeNode t;
  int check = typecheck(arvore, table, &t);
  printf("\nchecking resulted in %d\n", check);
  print_type(&t);
  printf("\n");
  // libera(arvore);
  arvore = NULL;
  yylex_destroy();
  return ret;
}
