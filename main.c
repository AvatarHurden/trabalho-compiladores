/*
Função principal para realização da análise sintática.

Este arquivo será posterioremente substituído, não acrescente nada.
*/
#include <stdio.h>
#include "parser.tab.h" //arquivo gerado com bison -d parser.y
#include "tree.h"

void libera(void* arvore);

int main (int argc, char **argv)
{
  node* n = make_string("1234");
  printf("%s\n", n->value->string_node);
  return yyparse();
}
