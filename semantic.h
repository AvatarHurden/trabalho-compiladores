#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "tree.h"
#include "table.h"

/* Verificação de declarações */
#define ERR_UNDECLARED  100 //identificador não declarado
#define ERR_DECLARED    101 //identificador já declarado

/* Uso correto de identificadores */
#define ERR_VARIABLE    200 //identificador deve ser utilizado como variável
#define ERR_VECTOR      201 //identificador deve ser utilizado como vetor
#define ERR_FUNCTION    202 //identificador deve ser utilizado como função
#define ERR_USER        203 //identificador deve ser utilizado como de usuário

/* Tipos e tamanho de dados */
#define ERR_WRONG_TYPE  300 //tipos incompatíveis
#define ERR_STRING_TO_X 301 //coerção impossível de var do tipo string
#define ERR_CHAR_TO_X   302 //coerção impossível de var do tipo char
#define ERR_USER_TO_X   303 //coerção impossível de var do tipo de usuário

/* Argumentos e parâmetros */
#define ERR_MISSING_ARGS    400 //faltam argumentos
#define ERR_EXCESS_ARGS     401 //sobram argumentos
#define ERR_WRONG_TYPE_ARGS 402 //argumentos incompatíveis

/* Verificação de tipos em comandos */
#define ERR_WRONG_PAR_INPUT  500 //parâmetro não é identificador
#define ERR_WRONG_PAR_OUTPUT 501 //parâmetro não é literal string ou expressão
#define ERR_WRONG_PAR_RETURN 502 //parâmetro não é expressão compatível com tipo do retorno

int typecheck(Node* node, SymbolsTable* table, TypeNode* out);
