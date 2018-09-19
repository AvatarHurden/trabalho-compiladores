#include "tree.h"
#include <string.h>

// Private functions

node* make_node(node_type type) {
  node* n = (node*) malloc(sizeof(node));
  n->type = type;
  n->value = malloc(sizeof(union node_value));
  return n;
}

void free_node(node* node) {
  free(node->value);
  free(node);
}

// Deletion Function

void libera(node* node) {
  delete(node);
}

void delete(node* node) {
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
    default:
      printf("Not implemented\n");
  }
  // Free the actual node (and value)
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
