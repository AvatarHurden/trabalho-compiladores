#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct node;

typedef enum {
  INT,
  FLOAT,
  BOOL,
  CHAR,
  STRING,
  IF,
  WHILE,
  DO_WHILE,
  BIN_OP
} node_type;

typedef enum {
  ADD,
  SUBTRACT,
  MULTIPLY,
  DIVIDE,
  MODULO,
  POW,
  GREATER,
  LESS_THAN,
  GREATER_EQUAL,
  LESS_EQUAL,
  EQUAL,
  NOT_EQUAL,
  AND,
  OR
} bin_op_type;

typedef enum {
  NOT,
  MINUS,
  PLUS,
  POINTER,
  ADDRESS,
  VALUE,
  EVAL_BOOL,
  HASH
} un_op_type;

typedef struct {
  struct node* cond_node;
  struct node* then_node;
  struct node* else_node;
} if_node;

typedef struct {
  struct node* cond_node;
  struct node* body_node;
} while_node;

typedef struct {
  struct node* left_node;
  struct node* right_node;
  bin_op_type type;
} bin_op_node;

typedef struct {
  struct node* value_node;
  un_op_type type;
} un_op_node;

typedef union {
  int int_node;
  float float_node;
  bool bool_node;
  char char_node;
  char* string_node;

  bin_op_node* bin_op_node;
  un_op_node* un_op_node;

  if_node* if_node;
  while_node* while_node;
  while_node* do_while_node;
} node_value;

typedef struct {
  node_type type;
  node_value* value;
} node;

int main() {
  node* a = (node*) malloc(sizeof(node));
  a->type = INT;
  if_node* n = (if_node*) malloc(sizeof(if_node));
  a->value = (node_value*) malloc(sizeof(node_value));
  a->value->int_node = 4;

  switch (a->type) {
    case IF:
      printf("is if with value %d\n", 4);
      break;
    case INT:
      printf("is int with value %d\n", a->value->int_node);
      break;
    default:
      printf("is other value\n");
  }

}
