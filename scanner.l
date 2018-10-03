%{
#include "parser.tab.h"

int column = 1;
void update_token_position();

%}

ALFABETICO [a-zA-Z_]
NUMERICO [0-9]
ID {ALFABETICO}({ALFABETICO}|{NUMERICO})*
INT {NUMERICO}+
FLOAT {INT}"."{NUMERICO}+([eE][+-]?{INT})?
CHAR '.'
STRING \"[^\n\"]*\"

WHITESPACE [ \t\n]+
LINE_COMMENT "//".*
BLOCK_COMMENT_START "/*"
BLOCK_COMMENT_END "*/"

%option yylineno

%x BLOCK_COMMENT

%%

{WHITESPACE} { update_token_position(); }
{LINE_COMMENT} { update_token_position(); }

{BLOCK_COMMENT_START} {
  update_token_position();
  BEGIN(BLOCK_COMMENT);
}

<BLOCK_COMMENT>{
  {BLOCK_COMMENT_END} {
    update_token_position();
    BEGIN(INITIAL);
  }

  "\n" { update_token_position(); }
  . { update_token_position(); }
}

"int" {
  update_token_position();
  yylval.token.category = TYPE_KEYWORD;
  yylval.token.value.type_keyword = INT_T;
  return TK_PR_INT;
}

"float" {
  update_token_position();
  yylval.token.category = TYPE_KEYWORD;
  yylval.token.value.type_keyword = FLOAT_T;
  return TK_PR_FLOAT;
}

"bool" {
  update_token_position();
  yylval.token.category = TYPE_KEYWORD;
  yylval.token.value.type_keyword = BOOL_T;
  return TK_PR_BOOL;
}

"char" {
  update_token_position();
  yylval.token.category = TYPE_KEYWORD;
  yylval.token.value.type_keyword = CHAR_T;
  return TK_PR_CHAR;
}

"string" {
  update_token_position();
  yylval.token.category = TYPE_KEYWORD;
  yylval.token.value.type_keyword = STRING_T;
  return TK_PR_STRING;
}

"if" {
  update_token_position();
  return TK_PR_IF;
}

"then" {
  update_token_position();
  return TK_PR_THEN;
}

"else" {
  update_token_position();
  return TK_PR_ELSE;
}

"while" {
  update_token_position();
  return TK_PR_WHILE;
}

"do" {
  update_token_position();
  return TK_PR_DO;
}

"input" {
  update_token_position();
  return TK_PR_INPUT;
}

"output" {
  update_token_position();
  return TK_PR_OUTPUT;
}

"return" {
  update_token_position();
  return TK_PR_RETURN;
}

"const" {
  update_token_position();
  return TK_PR_CONST;
}

"static" {
  update_token_position();
  return TK_PR_STATIC;
}

"foreach" {
  update_token_position();
  return TK_PR_FOREACH;
}

"for" {
  update_token_position();
  return TK_PR_FOR;
}

"switch" {
  update_token_position();
  return TK_PR_SWITCH;
}

"case" {
  update_token_position();
  return TK_PR_CASE;
}

"break" {
  update_token_position();
  return TK_PR_BREAK;
}

"continue" {
  update_token_position();
  return TK_PR_CONTINUE;
}

"class" {
  update_token_position();
  return TK_PR_CLASS;
}

"private" {
  update_token_position();
  yylval.token.category = SCOPE_KEYWORD;
  yylval.token.value.scope = PRIVATE;
  return TK_PR_PRIVATE;
}

"public" {
  update_token_position();
  yylval.token.category = SCOPE_KEYWORD;
  yylval.token.value.scope = PUBLIC;
  return TK_PR_PUBLIC;
}

"protected" {
  update_token_position();
  yylval.token.category = SCOPE_KEYWORD;
  yylval.token.value.scope = PROTECTED;
  return TK_PR_PROTECTED;
}

"," |
";" |
":" |
"(" |
")" |
"[" |
"]" |
"{" |
"}" |
"+" |
"-" |
"|" |
"?" |
"*" |
"/" |
"<" |
">" |
"=" |
"!" |
"&" |
"%" |
"#" |
"^" |
"." |
"$" {
  update_token_position();
  yylval.token.category = SPECIAL_CHAR;
  yylval.token.value.special_char = yytext[0];
  return yytext[0];
}

"<=" {
  update_token_position();
  yylval.token.category = BINARY_OPERATOR;
  yylval.token.value.binary_operator = LESS_EQUAL;
  return TK_OC_LE;
}

">=" {
  update_token_position();
  yylval.token.category = BINARY_OPERATOR;
  yylval.token.value.binary_operator = GREATER_EQUAL;
  return TK_OC_GE;
}

"==" {
  update_token_position();
  yylval.token.category = BINARY_OPERATOR;
  yylval.token.value.binary_operator = EQUAL;
  return TK_OC_EQ;
}

"!=" {
  update_token_position();
  yylval.token.category = BINARY_OPERATOR;
  yylval.token.value.binary_operator = NOT_EQUAL;
  return TK_OC_NE;
}

"&&" {
  update_token_position();
  yylval.token.category = BINARY_OPERATOR;
  yylval.token.value.binary_operator = AND;
  return TK_OC_AND;
}

"||" {
  update_token_position();
  yylval.token.category = BINARY_OPERATOR;
  yylval.token.value.binary_operator = OR;
  return TK_OC_OR;
}

">>" {
  update_token_position();
  return TK_OC_SR;
}

"<<" {
  update_token_position();
  return TK_OC_SL;
}

"%>%" {
  update_token_position();
  yylval.token.category = BINARY_OPERATOR;
  yylval.token.value.binary_operator = FORWARD_PIPE;
  return TK_OC_FORWARD_PIPE;
}

"%|%" {
  update_token_position();
  yylval.token.category = BINARY_OPERATOR;
  yylval.token.value.binary_operator = BASH_PIPE;
  return TK_OC_BASH_PIPE;
}

{INT} {
  update_token_position();
  yylval.token.category = INT_LITERAL;
  yylval.token.value.int_literal = atoi(yytext);
  return TK_LIT_INT;
}

{FLOAT} {
  update_token_position();
  yylval.token.category = FLOAT_LITERAL;
  yylval.token.value.float_literal = atof(yytext);
  return TK_LIT_FLOAT;
}

"true" {
  update_token_position();
  yylval.token.category = BOOL_LITERAL;
  yylval.token.value.bool_literal = true;
  return TK_LIT_TRUE;
}

"false" {
  update_token_position();
  yylval.token.category = BOOL_LITERAL;
  yylval.token.value.bool_literal = false;
  return TK_LIT_FALSE;
}

{CHAR} {
  update_token_position();
  yylval.token.category = CHAR_LITERAL;
  yylval.token.value.char_literal = yytext[1];
  return TK_LIT_CHAR;
}

{STRING} {
  update_token_position();
  yylval.token.category = STRING_LITERAL;
  yylval.token.value.string_literal = strndup(yytext + 1, strlen(yytext) - 2);
  return TK_LIT_STRING;
}

{ID} {
  update_token_position();
  yylval.token.category = IDENTIFIER;
  yylval.token.value.identifier = strdup(yytext);
  return TK_IDENTIFICADOR;
}

. {
  return TOKEN_ERRO;
}

%%

void update_token_position() {
    int i;
    for (i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            column = 1;
        } else {
            column++;
        }
    }

    yylval.token.line = yylineno;
    yylval.token.column = column;
}

int get_line_number() {
  return yylineno;
}

int get_column_number() {
  return column;
}
