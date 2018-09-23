%{
#include "parser.tab.h"

int column = 1;
void update_column();

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

{WHITESPACE} { update_column(); }
{LINE_COMMENT} { update_column(); }

{BLOCK_COMMENT_START} {
  update_column();
  BEGIN(BLOCK_COMMENT);
}

<BLOCK_COMMENT>{
  {BLOCK_COMMENT_END} {
    update_column();
    BEGIN(INITIAL);
  }

  "\n" { update_column(); }
  . { update_column(); }
}

"int" {
  update_column();
  return TK_PR_INT;
}

"float" {
  update_column();
  return TK_PR_FLOAT;
}

"bool" {
  update_column();
  return TK_PR_BOOL;
}

"char" {
  update_column();
  return TK_PR_CHAR;
}

"string" {
  update_column();
  return TK_PR_STRING;
}

"if" {
  update_column();
  return TK_PR_IF;
}

"then" {
  update_column();
  return TK_PR_THEN;
}

"else" {
  update_column();
  return TK_PR_ELSE;
}

"while" {
  update_column();
  return TK_PR_WHILE;
}

"do" {
  update_column();
  return TK_PR_DO;
}

"input" {
  update_column();
  return TK_PR_INPUT;
}

"output" {
  update_column();
  return TK_PR_OUTPUT;
}

"return" {
  update_column();
  return TK_PR_RETURN;
}

"const" {
  update_column();
  return TK_PR_CONST;
}

"static" {
  update_column();
  return TK_PR_STATIC;
}

"foreach" {
  update_column();
  return TK_PR_FOREACH;
}

"for" {
  update_column();
  return TK_PR_FOR;
}

"switch" {
  update_column();
  return TK_PR_SWITCH;
}

"case" {
  update_column();
  return TK_PR_CASE;
}

"break" {
  update_column();
  return TK_PR_BREAK;
}

"continue" {
  update_column();
  return TK_PR_CONTINUE;
}

"class" {
  update_column();
  return TK_PR_CLASS;
}

"private" {
  update_column();
  return TK_PR_PRIVATE;
}

"public" {
  update_column();
  return TK_PR_PUBLIC;
}

"protected" {
  update_column();
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
  update_column();
  return yytext[0];
}

"<=" {
  update_column();
  return TK_OC_LE;
}

">=" {
  update_column();
  return TK_OC_GE;
}

"==" {
  update_column();
  return TK_OC_EQ;
}

"!=" {
  update_column();
  return TK_OC_NE;
}

"&&" {
  update_column();
  return TK_OC_AND;
}

"||" {
  update_column();
  return TK_OC_OR;
}

">>" {
  update_column();
  return TK_OC_SR;
}

"<<" {
  update_column();
  return TK_OC_SL;
}

"%>%" {
  update_column();
  return TK_OC_FORWARD_PIPE;
}

"%|%" {
  update_column();
  return TK_OC_BASH_PIPE;
}

{INT} {
  update_column();
  return TK_LIT_INT;
}

{FLOAT} {
  update_column();
  return TK_LIT_FLOAT;
}

"true" {
  update_column();
  return TK_LIT_TRUE;
}

"false" {
  update_column();
  return TK_LIT_FALSE;
}

{CHAR} {
  update_column();
  return TK_LIT_CHAR;
}

{STRING} {
  update_column();
  return TK_LIT_STRING;
}

{ID} {
  update_column();
  return TK_IDENTIFICADOR;
}

. {
  return TOKEN_ERRO;
}

%%

void update_column() {
    int i;
    for (i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            column = 1;
        } else {
            column++;
        }
    }
}

int get_line_number() {
  return yylineno;
}

int get_column_number() {
  return column;
}
