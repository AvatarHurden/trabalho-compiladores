#include "catch.hpp"

extern "C" {
#include "../tokens.h"
#include "../lex.yy.h"
}

TEST_CASE( "Reserved words", "[scanner]" )
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
