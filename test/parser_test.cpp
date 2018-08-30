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

TEST_CASE("Global Variable Declarations")
{

    // Positive tests

    SECTION("Basic Variable") {
        yy_scan_string("a int;");
        REQUIRE(yyparse() == 0);
    }

    SECTION("New Type Varialbe") {
        yy_scan_string("a Complex;");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Array Variable") {
        yy_scan_string("a[5] char;");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Array Variable Negative Value") {
        yy_scan_string("a[-5] char;");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests

    SECTION("No Semicolon") {
        yy_scan_string("int a");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Missing type") {
        yy_scan_string("a;");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Missing identifier") {
        yy_scan_string("int;");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Wrong order") {
        yy_scan_string("int a;");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Array Variable Float Value") {
        yy_scan_string("char a[5.0];");
        REQUIRE(yyparse() == 1);
    }
}

TEST_CASE("Function Declarations")
{

    // Positive tests

    SECTION("Empty Parameters") {
        yy_scan_string("int f() {}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Static Function") {
        yy_scan_string("static char foo() {}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("One Parameter") {
        yy_scan_string("string fas(char c) {}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Multiple Parameters") {
        yy_scan_string("bool f_4(int a, char b) {}");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests

    SECTION("Finish With Semicolon") {
        yy_scan_string("int f() {};");
        REQUIRE(yyparse() == 1);
    }

    SECTION("No Body") {
        yy_scan_string("int f()");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Array as return") {
        yy_scan_string("string[] f() {}");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Array as Parameter") {
        yy_scan_string("char f(int[] a) {}");
        REQUIRE(yyparse() == 1);
    }
}
