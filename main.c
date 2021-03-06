/*
Função principal para realização da E3.

Este arquivo não pode ser modificado.
*/
#include <stdio.h>
#include "lex.yy.h"
#include "semantic.h"
#include "iloc.h"
#include "parser.tab.h" //arquivo gerado com bison -d parser.y

void *arvore = NULL;
void descompila (void *arvore);
void libera (void *arvore);

int main (int argc, char **argv)
{
  int ret = yyparse();
  if (ret == 0) {
    //descompila (arvore);
    ret = check_program(arvore);
  }
  if (ret == 0) {
    generate_code(arvore);
  }
  libera(arvore);
  arvore = NULL;
  yylex_destroy();
  return ret;
}
