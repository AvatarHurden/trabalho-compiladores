%{
int yylex(void);
void yyerror (char const *s);
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

program: global_decl program | global_decl;

global_decl: new_type | global_var_or_function;

new_type: TK_PR_CLASS TK_IDENTIFICADOR '{' fields '}' ';';
fields: field ':' fields | field;
field: scope_opt base_type TK_IDENTIFICADOR;


global_var_or_function:
    TK_PR_STATIC type TK_IDENTIFICADOR function_params body // Static function
    | base_type TK_IDENTIFICADOR function_params body       // Function with base_type
    | TK_IDENTIFICADOR custom_type_var_or_function;         // Still undecided

custom_type_var_or_function:
    array_index static_opt type ';'                        // Array variable
    | TK_PR_STATIC type ';'                                // Static type variable
    | base_type ';'                                        // Base type variable
    | TK_IDENTIFICADOR custom_type_simple_var_or_function; // Undecided

custom_type_simple_var_or_function:
    ';'                             // Variable
    | function_params body;         // Function

function_params: '(' params_opt ')';
params_opt: params | %empty;
params: param ',' params | param;
param: const_opt type TK_IDENTIFICADOR;

body: block;

block: '{' commands '}';
commands: command ';' commands | %empty;

command:
    TK_IDENTIFICADOR local_id_start
  | local_var
  | flow_control
  | input
  | output
  | return
  | block
  | function_call;

local_id_start: TK_IDENTIFICADOR // Variable with user type
              | attr_kind; // Attribution

local_var:
    TK_PR_STATIC const_opt local_var_decl
  | TK_PR_CONST local_var_decl
  | base_type TK_IDENTIFICADOR init_opt;

local_var_decl:
  base_type TK_IDENTIFICADOR init_opt;
  | TK_IDENTIFICADOR TK_IDENTIFICADOR;

init_opt: TK_OC_LE lit_or_id | %empty;

attr_kind: '[' expression ']' field_access_opt '=' expression;
         |  field_access '=' expression;
field_access: '$' TK_IDENTIFICADOR;
field_access_opt: field_access | %empty;

flow_control: TK_LIT_INT;

input: TK_LIT_CHAR;

output: TK_LIT_TRUE;

return: TK_LIT_FALSE;

function_call: TK_LIT_STRING;

expression: TOKEN_ERRO;

%%

void yyerror(const char* msg) {
    printf("%s\n", msg);
}