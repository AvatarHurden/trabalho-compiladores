%{
#include <stdio.h>
#include "lex.yy.h"
int yylex(void);
void yyerror (char const *s);
extern int get_line_number();
%}

%token TK_PR_INT
%token TK_PR_FLOAT
%token TK_PR_BOOL
%token TK_PR_CHAR
%token TK_PR_STRING
%token TK_PR_IF
%token TK_PR_THEN
%token TK_PR_ELSE
%token TK_PR_WHILE
%token TK_PR_DO
%token TK_PR_INPUT
%token TK_PR_OUTPUT
%token TK_PR_RETURN
%token TK_PR_CONST
%token TK_PR_STATIC
%token TK_PR_FOREACH
%token TK_PR_FOR
%token TK_PR_SWITCH
%token TK_PR_CASE
%token TK_PR_BREAK
%token TK_PR_CONTINUE
%token TK_PR_CLASS
%token TK_PR_PRIVATE
%token TK_PR_PUBLIC
%token TK_PR_PROTECTED
%token TK_OC_LE
%token TK_OC_GE
%token TK_OC_EQ
%token TK_OC_NE
%token TK_OC_AND
%token TK_OC_OR
%token TK_OC_SL
%token TK_OC_SR
%token TK_OC_FORWARD_PIPE
%token TK_OC_BASH_PIPE
%token TK_LIT_INT
%token TK_LIT_FLOAT
%token TK_LIT_FALSE
%token TK_LIT_TRUE
%token TK_LIT_CHAR
%token TK_LIT_STRING
%token TK_IDENTIFICADOR
%token TOKEN_ERRO

%start program

%%

// Utilities

literal: TK_LIT_INT
       | TK_LIT_FLOAT
       | TK_LIT_CHAR
       | TK_LIT_STRING
       | TK_LIT_TRUE
       | TK_LIT_FALSE;

lit_or_id: literal | TK_IDENTIFICADOR;

base_type: TK_PR_INT
         | TK_PR_BOOL
         | TK_PR_CHAR
         | TK_PR_STRING
         | TK_PR_FLOAT;
type: base_type | TK_IDENTIFICADOR;

scope: TK_PR_PRIVATE | TK_PR_PUBLIC | TK_PR_PROTECTED;
scope_opt: scope | %empty;

static_opt: TK_PR_STATIC | %empty;
const_opt: TK_PR_CONST | %empty;

array_index: '[' TK_LIT_INT ']';

// Grammar

program: global_declarations | %empty;

global_declarations: global_declaration global_declarations | global_declaration;

global_declaration: new_type | global_var | function;

new_type: TK_PR_CLASS TK_IDENTIFICADOR '[' field_list ']' ';';
field_list: field ':' field_list | field;
field: scope_opt base_type TK_IDENTIFICADOR;

global_var:
  TK_IDENTIFICADOR base_type ';'                     // Base type variable
  | TK_IDENTIFICADOR TK_IDENTIFICADOR ';'            // Custom type variable
  | TK_IDENTIFICADOR array_index static_opt type ';' // Array variable
  | TK_IDENTIFICADOR TK_PR_STATIC type ';';          // Static variable

function:
  base_type TK_IDENTIFICADOR function_params body            // Base type function
  | TK_IDENTIFICADOR TK_IDENTIFICADOR function_params body   // Custom type function
  | TK_PR_STATIC type TK_IDENTIFICADOR function_params body; // Static function

function_params: '(' param_list_opt ')';
param_list_opt: param_list | %empty;
param_list: param ',' param_list | param;
param: const_opt type TK_IDENTIFICADOR;

body: block;

block: '{' commands '}';
commands: command_or_case commands | %empty;

command_or_case: command ';'
               | TK_PR_CASE TK_LIT_INT ':';

command: command_with_comma | command_without_comma;

command_without_comma:
  local_var_decl
  | static_or_const_local_var_decl
  | local_var assign_or_shift
  | function_call
  | flow_control
  | return
  | input
  | TK_PR_BREAK
  | TK_PR_CONTINUE
  | block;

local_var_decl:
  base_type_local_var_decl
  | custom_type_local_var_decl;

base_type_local_var_decl:
  base_type TK_IDENTIFICADOR init_opt;

init_opt: TK_OC_LE lit_or_id | %empty;

custom_type_local_var_decl: TK_IDENTIFICADOR TK_IDENTIFICADOR;

static_or_const_local_var_decl:
  TK_PR_STATIC const_opt local_var_decl
  | TK_PR_CONST local_var_decl;

command_with_comma: output;

local_var: TK_IDENTIFICADOR array_or_field_access_opt;

array_or_field_access_opt: array_or_field_access | %empty;
array_or_field_access: '[' expression ']' field_access_opt;
        |  field_access;

field_access_opt: field_access | %empty;
field_access: '$' TK_IDENTIFICADOR;

assign_or_shift: '=' expression
               | TK_OC_SL expression
               | TK_OC_SR expression;

input: TK_PR_INPUT expression;
output: TK_PR_OUTPUT expression_list;

return: TK_PR_RETURN expression;

flow_control: if
            | foreach
            | for
            | do_while
            | while_do
            | switch;

if: TK_PR_IF '(' expression ')' TK_PR_THEN block else_opt;
else_opt: TK_PR_ELSE block | %empty;

foreach: TK_PR_FOREACH '(' TK_IDENTIFICADOR ':' expression_list ')' block;

for: TK_PR_FOR '(' commands_comma_separated ':' expression ':' commands_comma_separated ')' block;

commands_comma_separated: command_without_comma ',' commands_comma_separated | command_without_comma;

do_while: TK_PR_DO block TK_PR_WHILE '(' expression ')';

while_do: TK_PR_WHILE '(' expression ')' TK_PR_DO block;

switch: TK_PR_SWITCH '(' expression ')' block;

function_call:
  TK_IDENTIFICADOR function_args pipe_op function_call
  | TK_IDENTIFICADOR function_args;

function_args: '(' argument_list_opt ')';
argument_list_opt: argument_list | %empty;
argument_list: argument ',' argument_list | argument;
argument: expression | '.';

pipe_op: TK_OC_BASH_PIPE | TK_OC_FORWARD_PIPE;

expression_list: expression ',' expression_list | expression;
expression:
   operand
  | operand binary_operator expression
  | operand '?' expression ':' expression;

operand: unary_operator expression_element | expression_element;

expression_element:
  local_var
  | function_call
  | TK_LIT_INT
  | TK_LIT_FLOAT
  | TK_LIT_TRUE
  | TK_LIT_FALSE
  | TK_LIT_CHAR
  | TK_LIT_STRING
  | '(' expression ')';

unary_operator:
  '!'
  | '-'
  | '+'
  | '*'
  | '&'
  | '?'
  | '#';

binary_operator: '+'
  | '-'
  | '*'
  | '/'
  | '%'
  | '|'
  | '&'
  | '^'
  | '>'
  | '<'
  | '!'
  | TK_OC_GE
  | TK_OC_LE
  | TK_OC_EQ
  | TK_OC_NE
  | TK_OC_AND
  | TK_OC_OR;

%%

void yyerror(const char* msg) {
    char error_msg[] = "%s: Unexpected '%s' found near line %d\n";
    fprintf(stderr, error_msg, msg, yytext, get_line_number());
}
