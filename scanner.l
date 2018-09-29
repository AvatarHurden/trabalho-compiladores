%{
#include "parser.tab.h"

void assign_token_value(TokenCategory category, void* value);

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
  assign_token_value(TYPE_KEYWORD, INT_T);
  return TK_PR_INT;
}

"float" {
  update_column();
  assign_token_value(TYPE_KEYWORD, FLOAT_T);
  return TK_PR_FLOAT;
}

"bool" {
  update_column();
  assign_token_value(TYPE_KEYWORD, BOOL_T);
  return TK_PR_BOOL;
}

"char" {
  update_column();
  assign_token_value(TYPE_KEYWORD, CHAR_T);
  return TK_PR_CHAR;
}

"string" {
  update_column();
  assign_token_value(TYPE_KEYWORD, STRING_T);
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
  assign_token_value(SCOPE_KEYWORD, PRIVATE);
  return TK_PR_PRIVATE;
}

"public" {
  update_column();
  assign_token_value(SCOPE_KEYWORD, PUBLIC);
  return TK_PR_PUBLIC;
}

"protected" {
  update_column();
  assign_token_value(SCOPE_KEYWORD, PROTECTED);
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
  assign_token_value(SPECIAL_CHAR, yytext[0]);
  update_column();
  return yytext[0];
}

"<=" {
  assign_token_value(BINARY_OPERATOR, LESS_EQUAL);
  update_column();
  return TK_OC_LE;
}

">=" {
  assign_token_value(BINARY_OPERATOR, GREATER_EQUAL);
  update_column();
  return TK_OC_GE;
}

"==" {
  assign_token_value(BINARY_OPERATOR, EQUAL);
  update_column();
  return TK_OC_EQ;
}

"!=" {
  assign_token_value(BINARY_OPERATOR, NOT_EQUAL);
  update_column();
  return TK_OC_NE;
}

"&&" {
  assign_token_value(BINARY_OPERATOR, AND);
  update_column();
  return TK_OC_AND;
}

"||" {
  assign_token_value(BINARY_OPERATOR, OR);
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
  assign_token_value(BINARY_OPERATOR, FORWARD_PIPE);
  update_column();
  return TK_OC_FORWARD_PIPE;
}

"%|%" {
  assign_token_value(BINARY_OPERATOR, BASH_PIPE);
  update_column();
  return TK_OC_BASH_PIPE;
}

{INT} {
  assign_token_value(INT_LITERAL, NULL);
  update_column();
  return TK_LIT_INT;
}

{FLOAT} {
  assign_token_value(FLOAT_LITERAL, NULL);
  update_column();
  return TK_LIT_FLOAT;
}

"true" {
  assign_token_value(BOOL_LITERAL, true);
  update_column();
  return TK_LIT_TRUE;
}

"false" {
  assign_token_value(BOOL_LITERAL, false);
  update_column();
  return TK_LIT_FALSE;
}

{CHAR} {
  assign_token_value(CHAR_LITERAL, NULL);
  update_column();
  return TK_LIT_CHAR;
}

{STRING} {
  assign_token_value(STRING_LITERAL, NULL);
  update_column();
  return TK_LIT_STRING;
}

{ID} {
  assign_token_value(IDENTIFIER, NULL);
  update_column();
  return TK_IDENTIFICADOR;
}

. {
  return TOKEN_ERRO;
}

%%

void assign_token_value(TokenCategory category, void* value) {
    yylval.token.line = yylineno;
    yylval.token.column = column;
    yylval.token.category = category;

    switch (category) {
		case TYPE_KEYWORD:
			yylval.token.value.type_keyword = value;
			break;
		case SCOPE_KEYWORD:
			yylval.token.value.scope = value;
			break;
		case SPECIAL_CHAR:
			yylval.token.value.special_char = value;
			break;
		case BINARY_OPERATOR:
			yylval.token.value.binary_operator = value;
			break;
    case IDENTIFIER:
        yylval.token.value.identifier = strdup(yytext);
        break;
    case INT_LITERAL:
        yylval.token.value.int_literal = atoi(yytext);
    case FLOAT_LITERAL:
        yylval.token.value.float_literal = atof(yytext);
        break;
    case CHAR_LITERAL:
        yylval.token.value.char_literal = yytext[1];
        break;
    case STRING_LITERAL:
        yylval.token.value.string_literal = strndup(yytext + 1, strlen(yytext) - 2);
        break;
    case BOOL_LITERAL:
        yylval.token.value.bool_literal = value;
        break;
    default:
        break;
    }
}

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
