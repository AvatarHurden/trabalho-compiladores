#include "tree.h"
#include <string.h>

// Private functions

Node* make_node(NodeType type) {
  Node* n = (Node*) malloc(sizeof(Node));
  n->type = type;
  n->value = malloc(sizeof(union NodeValue));
  n->next = NULL;
  return n;
}

void free_node(Node* node) {
  free(node->value);
  free(node);
}

// Deletion Functions

void libera(Node* node) {
  delete(node);
}

void delete_type(TypeNode* type) {
  if (type == NULL)
    return;
  if (type->name != NULL)
    free(type->name);
  free(type);
}

void delete_param(ParamNode* node) {
  if (node == NULL)
    return;
  delete_type(node->type);
  delete_param(node->next);
  free(node->identifier);
  free(node);
}

void delete_field(FieldNode* node) {
  if (node == NULL)
    return;
  delete_type(node->type);
  delete_field(node->next);
  free(node->identifier);
  free(node);
}

void delete(Node* node) {
  if (node == NULL)
    return;
  // Type-specific internal frees
  switch (node->type) {
    case INT:
    case FLOAT:
    case BOOL:
    case CHAR:
      break;
    case STRING:
      free(node->value->string_node);
      break;
    case BIN_OP:
      delete(node->value->bin_op_node.left);
      delete(node->value->bin_op_node.right);
      break;
    case UN_OP:
      delete(node->value->un_op_node.value);
      break;
    case GLOBAL_VAR_DECL:
      delete_type(node->value->global_var_node.type);
      free(node->value->global_var_node.identifier);
    case TYPE_DECL:
      free(node->value->type_decl_node.identifier);
      delete_field(node->value->type_decl_node.field);
    case FUNCTION_DECL:
      delete_type(node->value->function_decl_node.type);
      free(node->value->function_decl_node.identifier);
      delete_param(node->value->function_decl_node.param);
      delete(node->value->function_decl_node.body);
      break;
    default:
      printf("Not implemented: %d\n", node->type);
  }
  // Free the actual node (and value)
  delete(node->next);
  free_node(node);
}

// Construction Functions

Node* make_int(int value) {
  Node* n = make_node(INT);
  n->value->int_node = value;
  return n;
}

Node* make_float(float value) {
  Node* n = make_node(FLOAT);
  n->value->float_node = value;
  return n;
}

Node* make_bool(bool value) {
  Node* n = make_node(BOOL);
  n->value->bool_node = value;
  return n;
}

Node* make_char(char value) {
  Node* n = make_node(CHAR);
  n->value->char_node = value;
  return n;
}

Node* make_string(char* value) {
  Node* n = make_node(STRING);
  n->value->string_node = strdup(value);
  return n;
}

Node* make_bin_op(Node* left, BinOpType type, Node* right) {
  Node* n = make_node(BIN_OP);
  n->value->bin_op_node.left = left;
  n->value->bin_op_node.right = right;
  n->value->bin_op_node.type = type;
  return n;
}

Node* make_un_op(Node* value, UnOpType type) {
  Node* n = make_node(UN_OP);
  n->value->un_op_node.value = value;
  n->value->un_op_node.type = type;
  return n;
}

TypeNode* make_type(TypeType kind, char* name) {
  TypeNode* n = (TypeNode*) malloc(sizeof(TypeNode));
  n->type = kind;
  if (kind == CUSTOM_T)
    n->name = strdup(name);
  else
    n->name = NULL;
  return n;
}

Node* make_global_var(TypeNode* type, char* id, bool is_static, int array_size) {
  Node* n = make_node(GLOBAL_VAR_DECL);
  n->value->global_var_node.type = type;
  n->value->global_var_node.identifier = strdup(id);
  n->value->global_var_node.is_static = is_static;
  n->value->global_var_node.array_size = array_size;
  return n;
}

FieldNode* make_field(Scope scope, TypeNode* type, char* id, FieldNode* next) {
  FieldNode* n = (FieldNode*) malloc(sizeof(FieldNode));
  n->scope = scope;
  n->type = type;
  n->identifier = strdup(id);
  n->next = next;
  return n;
}

Node* make_type_decl(char* id, FieldNode* field) {
  Node* n = make_node(TYPE_DECL);
  n->value->type_decl_node.identifier = strdup(id);
  n->value->type_decl_node.field = field;
  return n;
}

ParamNode* make_param(bool is_const, TypeNode* type, char* id, ParamNode* next) {
  ParamNode* n = (ParamNode*) malloc(sizeof(ParamNode));
  n->is_const = is_const;
  n->type = type;
  n->identifier = strdup(id);
  n->next = next;
  return n;
}

Node* make_function_decl(TypeNode* type, char* id, bool is_static, ParamNode* param, Node* body) {
  Node* n = make_node(FUNCTION_DECL);
  n->value->function_decl_node.type = type;
  n->value->function_decl_node.identifier = strdup(id);
  n->value->function_decl_node.is_static = is_static;
  n->value->function_decl_node.param = param;
  n->value->function_decl_node.body = body;
  return n;
}
