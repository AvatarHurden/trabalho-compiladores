%{
#include "tokens.h"
%}

%option yylineno

%%

"int" {
  return TK_PR_INT;
}

%%

int get_line_number() {
  return yylineno;
}
