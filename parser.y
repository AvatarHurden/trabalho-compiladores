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

base_type: TK_PR_INT
         | TK_PR_BOOL
         | TK_PR_CHAR
         | TK_PR_STRING
         | TK_PR_FLOAT;

scope: TK_PR_PRIVATE | TK_PR_PUBLIC | TK_PR_PROTECTED;
scope_opt: scope | %empty;


// Grammar

program: global_decl program | global_decl;

global_decl: global_var | new_type | function;

new_type: TK_PR_CLASS TK_IDENTIFICADOR '{' fields '}' ';';
fields: field ':' fields | field;
field: scope_opt base_type TK_IDENTIFICADOR;

global_var: TOKEN_ERRO;

function: TOKEN_ERRO;

%%

void yyerror(const char* msg) {
    printf("%s\n", msg);
}
