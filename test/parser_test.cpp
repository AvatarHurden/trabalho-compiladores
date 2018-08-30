#include "catch.hpp"

extern "C" {
#include "../parser.tab.h"
#include "../lex.yy.h"
#include <stdio.h>
}

struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char* str);

TEST_CASE("Type Declarations")
{

    // Positive tests

    SECTION("New Simple Type") {
        yy_scan_string("class Real { float value };");
        REQUIRE(yyparse() == 0);
    }

    SECTION("New Compound Type") {
        yy_scan_string("class Complex { float real : float imaginary };");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Allows Scope") {
        yy_scan_string("class Complex { private float real : float imaginary };");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests

    SECTION("Class Type in Field") {
        yy_scan_string("class Natural { Real value };");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Separator At End") {
        yy_scan_string("class Complex { float real : float imaginary : };");
        REQUIRE(yyparse() == 1);
    }

    SECTION("No Semicolon") {
        yy_scan_string("class Complex { float real : float imaginary }");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Only One Scope") {
        yy_scan_string("class Complex { private public float real };");
        REQUIRE(yyparse() == 1);
    }


}
