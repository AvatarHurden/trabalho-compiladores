#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct node;

// ENUMS

typedef enum {
  // Basic Values
  INT,
  FLOAT,
  BOOL,
  CHAR,
  STRING,
  // Operators
  BIN_OP,
  UN_OP,
  // Globals
  TYPE_DECL,
  GL_VAR_DECL,
  FUNCTION_DECL,
  // Commands
  VAR_DECL,
  ATTR,
  SHIFT_L,
  SHIFT_R,
  FUNCTION_CALL,
  RETURN,
  INPUT,
  OUTPUT,
  BREAK,
  CONTINUE,
  BLOCK,
  // Flow Controls
  IF,
  WHILE,
  DO_WHILE,
  SWITCH,
  FOR,
  FOREACH,
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

typedef enum {
  INT_T,
  FLOAT_T,
  CHAR_T,
  STRING_T,
  BOOL_T,
  CUSTOM_T
} type_type;

typedef enum {
  PRIVATE,
  PUBLIC,
  PROTECTED
} scope;

// Operator Nodes

typedef struct {
  struct node* left_node;
  struct node* right_node;
  bin_op_type type;
} bin_op_node;

typedef struct {
  struct node* value_node;
  un_op_type type;
} un_op_node;

// Helper Nodes

typedef struct {
  type_type type;
  char* name;
} type_node;

typedef struct {
  scope scope;
  type_node* type;
  char* identifier;
} field_node;

typedef struct {
  bool is_const;
  type_node* type;
  char* identifier;
} param_node;

// Top-Level Construction Nodes

typedef struct {
  type_node* type;
  char* identifier;
  bool is_static;
  int array_size;

  struct node* next;
} global_var_node;

typedef struct {
  char* identifier;
  int num_fields;
  field_node* fields;

  struct node* next;
} type_decl_node;

typedef struct {
  type_node* type;
  char* identifier;
  bool is_static;

  int num_params;
  param_node* params;

  struct node* body;

  struct node* next;
} function_decl_node;

// Command Nodes

typedef struct {
  type_node* type;
  char* identifier;
  bool is_static;
  bool is_const;

  struct node* init;

  struct node* next;
} local_var_node;

typedef struct {
  char* identifier;
  int index;
  char* field;

  struct node* value;

  struct node* next;
} attr_node;

typedef struct {
  char* identifier;

  int num_arguments;
  struct node* arguments;

  struct node* next;
} function_call_node;

typedef struct {
  int num_values;
  struct node* values;

  struct node* next;
} list_node;

typedef struct {
  int value;
  struct node* next;
} case_node;

typedef struct {
  struct node* values;

  struct node* next;
} block_node;

typedef struct {
  struct node* cond;
  struct node* then;
  struct node* else_node;

  struct node* next;
} if_node;

typedef struct {
  char* id;
  struct node* expressions;
  struct node* body;

  struct node* next;
} for_each_node;

typedef struct {
  struct node* initializers;
  struct node* expression;
  struct node* commands;

  struct node* body;

  struct node* next;
} for_node;

typedef struct {
  struct node* cond;
  struct node* body;

  struct node* next;
} while_node;

typedef struct {
  struct node* expression;

  struct node* body;

  struct node* next;
} switch_node;

// Actual nodes

typedef union {
  int int_node;
  float float_node;
  bool bool_node;
  char char_node;
  char* string_node;

  bin_op_node* bin_op_node;
  un_op_node* un_op_node;

  global_var_node* global_var_node;
  type_decl_node* type_decl_node;
  function_decl_node* function_decl_node;

  local_var_node* local_var_node;
  attr_node* attr_node;
  attr_node* shift_l_node;
  attr_node* shift_r_node;
  function_call_node* function_call_node;

  list_node* return_node;
  list_node* break_node;
  list_node* continue_node;
  case_node* case_node;

  list_node* input_node;
  list_node* output_node;

  block_node* block_node;

  if_node* if_node;
  while_node* while_node;
  while_node* do_while_node;
  switch_node* switch_node;
  for_node* for_node;
  for_each_node* for_each_node;
} node_value;

typedef struct {
  node_type type;
  node_value* value;
} node;

void delete(node* node);

node* make_int(int value);
node* make_float(float value);
node* make_bool(bool value);
node* make_char(char value);
node* make_string(char* value);
