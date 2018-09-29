%{
#include <stdio.h>
#include "lex.yy.h"
int yylex(void);
void yyerror (char const *s);

extern void *arvore;

extern int get_line_number();
extern int get_column_number();
%}



%code requires {
#include "tree.h"
}

%union {
  struct Token {
    int line;
    int column;
    TokenCategory category;
    TokenValue value;
  } token;
  Node* node;
  TypeNode* type;
  FieldNode* field;
  ParamNode* param;
  bool optional;
  UnOpType unary_operator;
}

%token <token.value.type_keyword> TK_PR_INT
%token <token.value.type_keyword> TK_PR_FLOAT
%token <token.value.type_keyword> TK_PR_BOOL
%token <token.value.type_keyword> TK_PR_CHAR
%token <token.value.type_keyword> TK_PR_STRING
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
%token <token> TK_PR_PRIVATE
%token <token> TK_PR_PUBLIC
%token <token> TK_PR_PROTECTED
%token <token> TK_OC_LE
%token <token> TK_OC_GE
%token <token> TK_OC_EQ
%token <token> TK_OC_NE
%token <token> TK_OC_AND
%token <token> TK_OC_OR
%token <token> TK_OC_SL
%token <token> TK_OC_SR
%token <token> TK_OC_FORWARD_PIPE
%token <token> TK_OC_BASH_PIPE
%token <token.value.int_literal> TK_LIT_INT
%token <token> TK_LIT_FLOAT
%token <token> TK_LIT_FALSE
%token <token> TK_LIT_TRUE
%token <token> TK_LIT_CHAR
%token <token> TK_LIT_STRING
%token <token.value.identifier> TK_IDENTIFICADOR
%token TOKEN_ERRO

%type <token> literal
%type <token> lit_or_id
%type <type> base_type
%type <type> type
%type <token.value.scope> scope
%type <token.value.scope> scope_opt
%type <optional> static_opt
%type <optional> const_opt
%type <token.value.int_literal> array_index

%type <node> program
%type <node> global_declarations
%type <node> global_declaration

%type <node> new_type;
%type <field> field_list;
%type <field> field;

%type <node> global_var

%type <node> function_declaration
%type <node> function_params
%type <node> param_list
%type <node> param

%type <node> body
%type <node> block
%type <node> commands
%type <node> command_or_case
%type <node> command
%type <node> command_with_comma
%type <node> command_without_comma
%type <node> local_var_decl
%type <node> simple_local_var_decl
%type <node> init_opt
%type <node> variable_access
%type <node> array_or_field_access_opt
%type <node> array_or_field_access
%type <node> field_access_opt
%type <token.value.identifier> field_access
%type <node> assign_or_shift
%type <node> input
%type <node> output
%type <node> return
%type <node> flow_control
%type <node> if
%type <node> else_opt
%type <node> foreach
%type <node> for
%type <node> commands_comma_separated
%type <node> do_while
%type <node> while_do
%type <node> switch
%type <node> function_call
%type <node> function
%type <node> argument_list_opt
%type <node> argument_list
%type <node> argument
%type <node> expression_list
%type <node> expression
%type <node> pipe_expression
%type <node> ternary_expression
%type <node> logical_or_expression
%type <node> logical_and_expression
%type <node> relational_expression
%type <node> additive_expression
%type <node> multiplicative_expression
%type <node> exponentiation_expression
%type <node> unary_expression
%type <node> operand
%type <token.value.binary_operator> pipe_operator
%type <token.value.binary_operator> logical_or_operator
%type <token.value.binary_operator> logical_and_operator
%type <token.value.binary_operator> relational_operator
%type <token.value.binary_operator> additive_operator
%type <token.value.binary_operator> multiplicative_operator
%type <unary_operator> unary_operator

%error-verbose

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

base_type: TK_PR_INT { $$ = make_type($1, NULL); }
         | TK_PR_BOOL { $$ = make_type($1, NULL); }
         | TK_PR_CHAR { $$ = make_type($1, NULL); }
         | TK_PR_STRING { $$ = make_type($1, NULL); }
         | TK_PR_FLOAT { $$ = make_type($1, NULL); };
type: base_type { $$ = $1; }
    | TK_IDENTIFICADOR { $$ = make_type(CUSTOM_T, $1); };

scope: TK_PR_PRIVATE | TK_PR_PUBLIC | TK_PR_PROTECTED;
scope_opt: scope { $$ = $1; }
         | %empty { $$ = NO_SCOPE; };

static_opt: TK_PR_STATIC { $$ = true; }
          | %empty { $$ = false; };
const_opt: TK_PR_CONST | %empty;

array_index: '[' TK_LIT_INT ']' { $$ = $2; };

// Grammar

program: global_declarations { arvore = $$; }
       | %empty { arvore = NULL; };

global_declarations: global_declaration global_declarations { $1->next = $2; $$ = $1; }
                   | global_declaration { $$ = $1; };

global_declaration: new_type { $$ = $1; }
                  | global_var { $$ = $1; }
                  | function_declaration;

new_type: TK_PR_CLASS TK_IDENTIFICADOR '[' field_list ']' ';' { $$ = make_type_decl($2, $4); };
field_list: field ':' field_list { $1->next = $3; $$ = $1; }
          | field { $$ = $1; };
field: scope_opt base_type TK_IDENTIFICADOR { $$ = make_field($1, $2, $3); };

global_var:
  TK_IDENTIFICADOR base_type ';'                     // Base type variable
      { $$ = make_global_var($2, $1, false, -1); }
  | TK_IDENTIFICADOR TK_IDENTIFICADOR ';'            // Custom type variable
      { $$ = make_global_var(make_type(CUSTOM_T, $2), $1, false, -1); }
  | TK_IDENTIFICADOR array_index static_opt type ';' // Array variable
      { $$ = make_global_var($4, $1, $3, $2); }
  | TK_IDENTIFICADOR TK_PR_STATIC type ';'          // Static variable
      { $$ = make_global_var($3, $1, true, -1); };

function_declaration:
  base_type TK_IDENTIFICADOR function_params body            // Base type function
  | TK_IDENTIFICADOR TK_IDENTIFICADOR function_params body   // Custom type function
  | TK_PR_STATIC type TK_IDENTIFICADOR function_params body; // Static function

function_params: '(' ')' | '(' param_list ')';
param_list: param ',' param_list | param;
param: const_opt type TK_IDENTIFICADOR;

body: block;

block: '{' commands '}';
commands: command_or_case commands | %empty;

command_or_case: command ';'
               | TK_PR_CASE TK_LIT_INT ':';

command: command_with_comma | command_without_comma;

command_with_comma: output;

command_without_comma:
  local_var_decl
  | variable_access assign_or_shift
  | function_call
  | flow_control
  | return
  | input
  | TK_PR_BREAK
  | TK_PR_CONTINUE
  | block;

local_var_decl:
  simple_local_var_decl
  | TK_PR_STATIC const_opt simple_local_var_decl
  | TK_PR_CONST simple_local_var_decl;

simple_local_var_decl:
  base_type TK_IDENTIFICADOR init_opt    // Base type local var
  | TK_IDENTIFICADOR TK_IDENTIFICADOR;   // Custom type local var

init_opt: TK_OC_LE lit_or_id | %empty;

variable_access: TK_IDENTIFICADOR array_or_field_access_opt;

array_or_field_access_opt: array_or_field_access | %empty;
array_or_field_access:
  '[' expression ']' field_access_opt;
  | field_access;

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
  function
  | function pipe_operator function_call;

function: TK_IDENTIFICADOR '(' argument_list_opt ')';

argument_list_opt: argument_list | %empty;
argument_list: argument ',' argument_list | argument;
argument: expression | '.';

expression_list: expression ',' expression_list | expression;
expression: pipe_expression;

pipe_operator: TK_OC_BASH_PIPE | TK_OC_FORWARD_PIPE;
pipe_expression:
  ternary_expression
  | pipe_expression pipe_operator ternary_expression;

ternary_expression:
  logical_or_expression
  | logical_or_expression '?' expression ':' ternary_expression;

logical_or_operator: TK_OC_OR | '|';
logical_or_expression:
  logical_and_expression
  | logical_or_expression logical_or_operator logical_and_expression;

logical_and_operator: TK_OC_AND | '&';
logical_and_expression:
  relational_expression
  | logical_and_expression logical_and_operator relational_expression;

relational_operator: TK_OC_EQ | TK_OC_NE | TK_OC_GE | TK_OC_LE | '>' | '<';
relational_expression:
  additive_expression
  | relational_expression relational_operator additive_expression;

additive_operator: '+' | '-';
additive_expression:
  multiplicative_expression
  | additive_expression additive_operator multiplicative_expression;

multiplicative_operator: '*' | '/' | '%';
multiplicative_expression:
  exponentiation_expression
  | multiplicative_expression multiplicative_operator exponentiation_expression;

exponentiation_expression:
  unary_expression
  | exponentiation_expression '^' unary_expression;

unary_operator: '!' | '-' | '+' | '*' | '&' | '?' | '#';
unary_expression:
  operand
  | unary_operator unary_expression;

operand:
  variable_access
  | function
  | literal
  | '(' expression ')';

%%

void yyerror(const char* msg) {
    char error_msg[] = "%s at line %d, column %d\n";
    fprintf(stderr, error_msg, msg, get_line_number(), get_column_number());
}
