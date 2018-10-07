#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "tree.h"

typedef struct Symbol {
  int line, column;
  int size;
  int nature;
  TypeNode* type;
  ParamNode* params;
  FieldNode* fields;
} Symbol;

enum Nature {
  LITERAL_INT,
  LITERAL_FLOAT,
  LITERAL_CHAR,
  LITERAL_STRING,
  LITERAL_BOOL,
  VARIABLE,
  FUNCTION,
  CLASS
 };
