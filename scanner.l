%{
#include "tokens.h"
%}

%%

"int" {
  return TK_PR_INT;
}

%%

int get_line_number() {
  return 0;
}
