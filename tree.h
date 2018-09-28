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
  VARIABLE,
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
  DOT,
  RETURN,
  INPUT,
  OUTPUT,
  BREAK,
  CONTINUE,
  CASE,
  BLOCK,
  // Flow Controls
  IF,
  WHILE,
  DO_WHILE,
  SWITCH,
  FOR,
  FOR_EACH,
} NodeType;

union NodeValue;

typedef struct Node {
  NodeType type;
  union NodeValue* value;
  struct Node* next;
} Node;

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
  OR,
  BASH_PIPE,
  FORWARD_PIPE
  } BinOpType;

typedef enum {
  NOT,
  MINUS,
  PLUS,
  POINTER,
  ADDRESS,
  VALUE,
  EVAL_BOOL,
  HASH
} UnOpType;

typedef enum {
  INT_T,
  FLOAT_T,
  CHAR_T,
  STRING_T,
  BOOL_T,
  CUSTOM_T
} TypeType;

typedef enum {
  PRIVATE,
  PUBLIC,
  PROTECTED,
  NO_SCOPE
} Scope;

// Operator Nodes

typedef struct {
  Node* left;
  Node* right;
  BinOpType type;
} BinOpNode;

typedef struct {
  Node* value;
  UnOpType type;
} UnOpNode;

// Helper Nodes

typedef struct {
  TypeType type;
  char* name;
} TypeNode;

typedef struct FieldNode {
  Scope scope;
  TypeNode* type;
  char* identifier;

  struct FieldNode* next;
} FieldNode;

typedef struct ParamNode {
  bool is_const;
  TypeNode* type;
  char* identifier;

  struct ParamNode* next;
} ParamNode;

// Top-Level Construction Nodes

typedef struct {
  TypeNode* type;
  char* identifier;
  bool is_static;
  int array_size;
} GlobalVarNode;

typedef struct {
  char* identifier;
  FieldNode* field;
} TypeDeclNode;

typedef struct {
  TypeNode* type;
  char* identifier;
  bool is_static;

  ParamNode* param;

  Node* body;
} FunctionDeclNode;

// Command Nodes

typedef struct {
  TypeNode* type;
  char* identifier;
  bool is_static;
  bool is_const;

  Node* init;
} LocalVarNode;

typedef struct {
  char* identifier;
  int index;
  char* field;
} VariableNode;

typedef struct {
  char* identifier;
  int index;
  char* field;

  Node* value;
} AttrNode;

typedef struct {
  char* identifier;

  Node* arguments;
} FunctionCallNode;

typedef struct {
  Node* value;
} ListNode;

typedef struct {
  Node* cond;
  Node* then;
  Node* else_node;
} IfNode;

typedef struct {
  char* id;
  Node* expression;
  Node* body;
} ForEachNode;

typedef struct {
  Node* initializers;
  Node* expressions;
  Node* commands;

  Node* body;
} ForNode;

typedef struct {
  Node* cond;
  Node* body;
} WhileNode;

typedef struct {
  Node* expression;
  Node* body;
} SwitchNode;

// Actual nodes

typedef union NodeValue {
  int int_node;
  float float_node;
  bool bool_node;
  char char_node;
  char* string_node;
  VariableNode var_node;

  BinOpNode bin_op_node;
  UnOpNode un_op_node;

  GlobalVarNode global_var_node;
  TypeDeclNode type_decl_node;
  FunctionDeclNode function_decl_node;

  LocalVarNode local_var_node;
  AttrNode attr_node;
  AttrNode shift_l_node;
  AttrNode shift_r_node;
  FunctionCallNode function_call_node;

  ListNode return_node;
  int case_node;

  ListNode input_node;
  ListNode output_node;

  ListNode block_node;

  IfNode if_node;
  WhileNode while_node;
  WhileNode do_while_node;
  SwitchNode switch_node;
  ForNode for_node;
  ForEachNode for_each_node;
} NodeValue;

typedef enum  {
  TYPE_KEYWORD,
  SCOPE_KEYWORD,
  SPECIAL_CHAR,
  BINARY_OPERATOR,
  IDENTIFIER,
  INT_LITERAL,
  FLOAT_LITERAL,
  CHAR_LITERAL,
  BOOL_LITERAL,
  STRING_LITERAL
} TokenCategory;

typedef union {
  TypeType type_keyword;
  Scope scope;
  char special_char;
  BinOpType binary_operator;
  char* identifier;
  int int_literal;
  float float_literal;
  char char_literal;
  bool bool_literal;
  char* string_literal;
} TokenValue;

void delete(Node* node);

Node* make_int(int value);
Node* make_float(float value);
Node* make_bool(bool value);
Node* make_char(char value);
Node* make_string(char* value);
Node* make_variable(char* id, int index, char* field);

Node* make_bin_op(Node* left, BinOpType type, Node* right);
Node* make_un_op(Node* value, UnOpType type);

TypeNode* make_type(TypeType kind, char* name);
FieldNode* make_field(Scope scope, TypeNode* type, char* id, FieldNode* next);
ParamNode* make_param(bool is_const, TypeNode* type, char* id, ParamNode* next);

Node* make_global_var(TypeNode* type, char* id, bool is_static, int array_size);
Node* make_type_decl(char* id, FieldNode* field);
Node* make_function_decl(TypeNode* type, char* id, bool is_static, ParamNode* param, Node* body);

Node* make_local_var(TypeNode* type, char* id, bool is_static, bool is_const, Node* init);
Node* make_attr(char* id, int index, char* field, Node* value);
Node* make_shift_l(char* id, int index, char* field, Node* value);
Node* make_shift_r(char* id, int index, char* field, Node* value);

Node* make_function_call(char* id, Node* arguments);
Node* make_dot();

Node* make_return(Node* value);
Node* make_break();
Node* make_continue();
Node* make_case(int value);

Node* make_input(Node* value);
Node* make_output(Node* value);

Node* make_block(Node* value);

Node* make_if(Node* cond, Node* then, Node* else_node);
Node* make_for_each(char* id, Node* expression, Node* body);
Node* make_for(Node* initializers, Node* expressions, Node* commands, Node* body);
Node* make_while(Node* cond, Node* body);
Node* make_do_while(Node* cond, Node* body);
Node* make_switch(Node* expression, Node* body);
