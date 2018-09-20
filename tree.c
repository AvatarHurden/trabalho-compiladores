#include "tree.h"
#include <string.h>

// Private functions

node* make_node(node_type type) {
  node* n = (node*) malloc(sizeof(node));
  n->type = type;
  n->value = malloc(sizeof(union node_value));
  n->next = NULL;
  return n;
}

void free_node(node* node) {
  free(node->value);
  free(node);
}

// Deletion Functions

void libera(node* node) {
  delete(node);
}

void delete_type(type_node* type) {
  if (type == NULL)
    return;
  if (type->name != NULL)
    free(type->name);
  free(type);
}

void delete_param(param_node* node) {
  if (node == NULL)
    return;
  delete_type(node->type);
  delete_param(node->next);
  free(node->identifier);
  free(node);
}

void delete_field(field_node* node) {
  if (node == NULL)
    return;
  delete_type(node->type);
  delete_field(node->next);
  free(node->identifier);
  free(node);
}

void delete(node* node) {
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

node* make_int(int value) {
  node* n = make_node(INT);
  n->value->int_node = value;
  return n;
}

node* make_float(float value) {
  node* n = make_node(FLOAT);
  n->value->float_node = value;
  return n;
}

node* make_bool(bool value) {
  node* n = make_node(BOOL);
  n->value->bool_node = value;
  return n;
}

node* make_char(char value) {
  node* n = make_node(CHAR);
  n->value->char_node = value;
  return n;
}

node* make_string(char* value) {
  node* n = make_node(STRING);
  n->value->string_node = strdup(value);
  return n;
}

node* make_bin_op(node* left, bin_op_type type, node* right) {
  node* n = make_node(BIN_OP);
  n->value->bin_op_node.left = left;
  n->value->bin_op_node.right = right;
  n->value->bin_op_node.type = type;
  return n;
}

node* make_un_op(node* value, un_op_type type) {
  node* n = make_node(UN_OP);
  n->value->un_op_node.value = value;
  n->value->un_op_node.type = type;
  return n;
}

type_node* make_type(type_type kind, char* name) {
  type_node* n = (type_node*) malloc(sizeof(type_node));
  n->type = kind;
  if (kind == CUSTOM_T)
    n->name = strdup(name);
  else
    n->name = NULL;
  return n;
}

node* make_global_var(type_node* type, char* id, bool is_static, int array_size) {
  node* n = make_node(GLOBAL_VAR_DECL);
  n->value->global_var_node.type = type;
  n->value->global_var_node.identifier = strdup(id);
  n->value->global_var_node.is_static = is_static;
  n->value->global_var_node.array_size = array_size;
  return n;
}

field_node* make_field(scope scope, type_node* type, char* id, field_node* next) {
  field_node* n = (field_node*) malloc(sizeof(field_node));
  n->scope = scope;
  n->type = type;
  n->identifier = strdup(id);
  n->next = next;
  return n;
}

node* make_type_decl(char* id, field_node* field) {
  node* n = make_node(TYPE_DECL);
  n->value->type_decl_node.identifier = strdup(id);
  n->value->type_decl_node.field = field;
  return n;
}

param_node* make_param(bool is_const, type_node* type, char* id, param_node* next) {
  param_node* n = (param_node*) malloc(sizeof(param_node));
  n->is_const = is_const;
  n->type = type;
  n->identifier = strdup(id);
  n->next = next;
  return n;
}

node* make_function_decl(type_node* type, char* id, bool is_static, param_node* param, node* body) {
  node* n = make_node(FUNCTION_DECL);
  n->value->function_decl_node.type = type;
  n->value->function_decl_node.identifier = strdup(id);
  n->value->function_decl_node.is_static = is_static;
  n->value->function_decl_node.param = param;
  n->value->function_decl_node.body = body;
  return n;
}
