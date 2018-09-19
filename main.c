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
  node* n2 = make_int(4);
  node* n3 = make_bin_op(n, MULTIPLY, n2);
  printf("%d\n", n3->value->bin_op_node.type);
  libera(n3);
  printf("%d\n", n2->value->int_node);
  return yyparse();
}
