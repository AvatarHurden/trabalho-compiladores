#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Definition of nodes

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
  GLOBAL_VAR_DECL,
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

union node_value;

typedef struct node {
  node_type type;
  union node_value* value;
  struct node* next;
} node;

// ENUMS

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
  node* left;
  node* right;
  bin_op_type type;
} bin_op_node;

typedef struct {
  node* value;
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
} global_var_node;

typedef struct {
  char* identifier;
  int num_fields;
  field_node* fields;
} type_decl_node;

typedef struct {
  type_node* type;
  char* identifier;
  bool is_static;

  int num_params;
  param_node* params;

  node* body;
} function_decl_node;

// Command Nodes

typedef struct {
  type_node* type;
  char* identifier;
  bool is_static;
  bool is_const;

  node* init;
} local_var_node;

typedef struct {
  char* identifier;
  int index;
  char* field;

  node* value;
} attr_node;

typedef struct {
  char* identifier;

  int num_arguments;
  node* arguments;
} function_call_node;

typedef struct {
  int num_values;
  node* values;
} list_node;

typedef struct {
  int value;
} case_node;

typedef struct {
  node* values;
} block_node;

typedef struct {
  node* cond;
  node* then;
  node* else_node;
} if_node;

typedef struct {
  char* id;
  node* expressions;
  node* body;
} for_each_node;

typedef struct {
  node* initializers;
  node* expression;
  node* commands;

  node* body;
} for_node;

typedef struct {
  node* cond;
  node* body;
} while_node;

typedef struct {
  node* expression;

  node* body;
} switch_node;

// Actual nodes

typedef union node_value {
  int int_node;
  float float_node;
  bool bool_node;
  char char_node;
  char* string_node;

  bin_op_node bin_op_node;
  un_op_node un_op_node;

  global_var_node global_var_node;
  type_decl_node type_decl_node;
  function_decl_node function_decl_node;

  local_var_node local_var_node;
  attr_node attr_node;
  attr_node shift_l_node;
  attr_node shift_r_node;
  function_call_node function_call_node;

  list_node return_node;
  int case_node;

  list_node input_node;
  list_node output_node;

  block_node block_node;

  if_node if_node;
  while_node while_node;
  while_node do_while_node;
  switch_node switch_node;
  for_node for_node;
  for_each_node for_each_node;
} node_value;


void delete(node* node);

node* make_int(int value);
node* make_float(float value);
node* make_bool(bool value);
node* make_char(char value);
node* make_string(char* value);

node* make_bin_op(node* left, bin_op_type type, node* right);
node* make_un_op(node* value, un_op_type type);

type_node* make_type(type_type kind, char* name);
field_node* make_field(scope scope, type_node* type, char* id);
param_node* make_param(bool is_const, type_node* type, char* id);

node* make_global_var(type_node* type, char* id, bool is_static, int array_size);
node* make_type_decl(char* id, int num_fields, field_node* fields);
node* make_function_decl(type_node* type, char* id, bool is_static, int num_params, param_node* params, node* body);
