#include "catch.hpp"

extern "C" {
#include "../tokens.h"
#include "../lex.yy.h"
}

TEST_CASE("Reserved words")
{
    SECTION("Types") {
        yy_scan_string("int float bool char string");
        REQUIRE(yylex() == TK_PR_INT);
        REQUIRE(yylex() == TK_PR_FLOAT);
        REQUIRE(yylex() == TK_PR_BOOL);
        REQUIRE(yylex() == TK_PR_CHAR);
        REQUIRE(yylex() == TK_PR_STRING);
    }

    SECTION ("Conditionals") {
        yy_scan_string("if then else switch case");
        REQUIRE(yylex() == TK_PR_IF);
        REQUIRE(yylex() == TK_PR_THEN);
        REQUIRE(yylex() == TK_PR_ELSE);
        REQUIRE(yylex() == TK_PR_SWITCH);
        REQUIRE(yylex() == TK_PR_CASE);
    }

    SECTION ("Loop control") {
        yy_scan_string("while for foreach continue break");
        REQUIRE(yylex() == TK_PR_WHILE);
        REQUIRE(yylex() == TK_PR_FOR);
        REQUIRE(yylex() == TK_PR_FOREACH);
        REQUIRE(yylex() == TK_PR_CONTINUE);
        REQUIRE(yylex() == TK_PR_BREAK);
    }

    SECTION ("Access modifiers") {
        yy_scan_string("public protected private");
        REQUIRE(yylex() == TK_PR_PUBLIC);
        REQUIRE(yylex() == TK_PR_PROTECTED);
        REQUIRE(yylex() == TK_PR_PRIVATE);
    }

    SECTION ("Other") {
        yy_scan_string("input output return const static class");
        REQUIRE(yylex() == TK_PR_INPUT);
        REQUIRE(yylex() == TK_PR_OUTPUT);
        REQUIRE(yylex() == TK_PR_RETURN);
        REQUIRE(yylex() == TK_PR_CONST);
        REQUIRE(yylex() == TK_PR_STATIC);
        REQUIRE(yylex() == TK_PR_CLASS);
    }
}

TEST_CASE("Identifiers")
{
    yy_scan_string("intx xint _int int_ int7 int7x");
    REQUIRE(yylex() == TK_IDENTIFICADOR);
    REQUIRE(yylex() == TK_IDENTIFICADOR);
    REQUIRE(yylex() == TK_IDENTIFICADOR);
    REQUIRE(yylex() == TK_IDENTIFICADOR);
    REQUIRE(yylex() == TK_IDENTIFICADOR);
    REQUIRE(yylex() == TK_IDENTIFICADOR);

    yy_scan_string("7int");
    REQUIRE(yylex() != TK_IDENTIFICADOR);
    REQUIRE(yylex() != TK_IDENTIFICADOR);
}

TEST_CASE("Literals")
{
    SECTION("Int") {
        yy_scan_string("207");
        REQUIRE(yylex() == TK_LIT_INT);
        REQUIRE(std::string(yytext) == "207");

        yy_scan_string("+207");
        REQUIRE(yylex() == TK_LIT_INT);
        REQUIRE(std::string(yytext) == "+207");

        yy_scan_string("-207");
        REQUIRE(yylex() == TK_LIT_INT);
        REQUIRE(std::string(yytext) == "-207");
    }

    SECTION("Float") {
        yy_scan_string("2.07");
        REQUIRE(yylex() == TK_LIT_FLOAT);
        REQUIRE(std::string(yytext) == "2.07");

        yy_scan_string("+2.07");
        REQUIRE(yylex() == TK_LIT_FLOAT);
        REQUIRE(std::string(yytext) == "+2.07");

        yy_scan_string("-2.07");
        REQUIRE(yylex() == TK_LIT_FLOAT);
        REQUIRE(std::string(yytext) == "-2.07");

        yy_scan_string("2.07e33");
        REQUIRE(yylex() == TK_LIT_FLOAT);
        REQUIRE(std::string(yytext) == "2.07e33");

        yy_scan_string("2.07e-33");
        REQUIRE(yylex() == TK_LIT_FLOAT);
        REQUIRE(std::string(yytext) == "2.07e-33");

        yy_scan_string("-2.07E+33");
        REQUIRE(yylex() == TK_LIT_FLOAT);
        REQUIRE(std::string(yytext) == "-2.07E+33");

        yy_scan_string("2.");
        REQUIRE(yylex() != TK_LIT_FLOAT);
        REQUIRE(std::string(yytext) != "2.07");

        yy_scan_string(".07");
        REQUIRE(yylex() != TK_LIT_FLOAT);
        REQUIRE(std::string(yytext) != "2.07");
    }

    SECTION("Bool") {
        yy_scan_string("true");
        REQUIRE(yylex() == TK_LIT_TRUE);
        REQUIRE(std::string(yytext) == "true");

        yy_scan_string("false");
        REQUIRE(yylex() == TK_LIT_FALSE);
        REQUIRE(std::string(yytext) == "false");

        yy_scan_string("True");
        REQUIRE(yylex() != TK_LIT_TRUE);
        REQUIRE(std::string(yytext) == "True");
    }

    SECTION("Char") {
        yy_scan_string("'a'");
        REQUIRE(yylex() == TK_LIT_CHAR);
        REQUIRE(std::string(yytext) == "'a'");

        yy_scan_string("' '");
        REQUIRE(yylex() == TK_LIT_CHAR);
        REQUIRE(std::string(yytext) == "' '");

        yy_scan_string("''");
        REQUIRE(yylex() != TK_LIT_CHAR);
        REQUIRE(std::string(yytext) != "''");

        yy_scan_string("'\\t'");
        REQUIRE(yylex() != TK_LIT_CHAR);
        REQUIRE(std::string(yytext) != "'\\t'");
    }

    SECTION("String") {
        SECTION("Regular") {
            yy_scan_string("\"abc\"");
            REQUIRE(yylex() == TK_LIT_STRING);
            REQUIRE(std::string(yytext) == "\"abc\"");
        }

        SECTION("Empty") {
            yy_scan_string("\"\"");
            REQUIRE(yylex() == TK_LIT_STRING);
            REQUIRE(std::string(yytext) == "\"\"");
        }
    }
}

TEST_CASE("Error token") {
    yy_scan_string("'abc'");
    REQUIRE(yylex() == TOKEN_ERRO);
}

TEST_CASE("Comments") {
    SECTION("Line comment"){
        yy_scan_string("int// ignore this \nfloat");
        REQUIRE(yylex() == TK_PR_INT);
        REQUIRE(std::string(yytext) == "int");
        REQUIRE(yylex() == TK_PR_FLOAT);
        REQUIRE(std::string(yytext) == "float");
    }

    SECTION("Block comment"){
        yy_scan_string("int/* ignore this\nand this\n*/float");
        REQUIRE(yylex() == TK_PR_INT);
        REQUIRE(std::string(yytext) == "int");
        REQUIRE(yylex() == TK_PR_FLOAT);
        REQUIRE(std::string(yytext) == "float");
    }
}