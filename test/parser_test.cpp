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
        yy_scan_string("class Real [ float value ];");
        REQUIRE(yyparse() == 0);
    }

    SECTION("New Compound Type") {
        yy_scan_string("class Complex [ float real : float imaginary ];");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Allows Scope") {
        yy_scan_string("class Complex [ private float real : float imaginary ];");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests

    SECTION("Class Type in Field") {
        yy_scan_string("class Natural [ Real value ];");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Separator At End") {
        yy_scan_string("class Complex [ float real : float imaginary : ];");
        REQUIRE(yyparse() == 1);
    }

    SECTION("No Semicolon") {
        yy_scan_string("class Complex [ float real : float imaginary ]");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Only One Scope") {
        yy_scan_string("class Complex [ private public float real ];");
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

TEST_CASE("Local Variable Declarations")
{

    // Positive tests

    SECTION("Simple variable") {
        yy_scan_string("int main() {"
                       " int a;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Custom type") {
        yy_scan_string("int main() {"
                       " Real a;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Static Const") {
        yy_scan_string("int main() {"
                       " static const int a;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Only Const") {
      yy_scan_string("int main() {"
                     " const Real a;"
                     "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Only Static") {
      yy_scan_string("int main() {"
                     " static int a;"
                     "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("With attribution literal") {
        yy_scan_string("int main() {"
                       " int a <= 4;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("With attribution identifier") {
        yy_scan_string("int main() {"
                       " int a <= b;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests

    SECTION("Const Static") {
        yy_scan_string("int main() {"
                       " const static Real a;"
                       "}");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Custom type with attribution") {
        yy_scan_string("int main() {"
                       " Real a <= 5;"
                       "}");
        REQUIRE(yyparse() == 1);
    }

}

TEST_CASE("Attributions")
{

    // Positive tests

    SECTION("Simple variable") {
        yy_scan_string("int main() {"
                       " a = 4;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Array variable") {
        yy_scan_string("int main() {"
                       " a[4] = 'a';"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Field variable") {
        yy_scan_string("int main() {"
                       " a$field = 3 + 4;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Array field variable") {
      yy_scan_string("int main() {"
                     " a[3+2]$field_2 = 5 > 2;"
                     "}");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests

    SECTION("Missing value") {
      yy_scan_string("int main() {"
                     " a = ;"
                     "}");
        REQUIRE(yyparse() == 1);
    }

}


TEST_CASE("Input and Output")
{

    // Positive tests

    SECTION("Simple Input") {
        yy_scan_string("int main() {"
                       " input 4 > 2 + 1;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Simple Output") {
        yy_scan_string("int main() {"
                       " output true | false;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Compound Output") {
        yy_scan_string("int main() {"
                       " output \"string\" & 3, 'a', -10 * +4;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests

    SECTION("Empty Output") {
        yy_scan_string("int main() {"
                       " output;"
                       "}");
        REQUIRE(yyparse() == 1);
    }
}


TEST_CASE("Function call")
{

    // Positive tests

    SECTION("Empty parameters") {
        yy_scan_string("int main() {"
                       " f();"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("One parameter") {
        yy_scan_string("int main() {"
                       " foo(false);"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Dot parameter") {
        yy_scan_string("int main() {"
                       " bar(.);"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Multiple parameters") {
        yy_scan_string("int main() {"
                       " bar(a, 4, ., 'c');"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests
}


TEST_CASE("Shifts")
{

    // Positive tests

    SECTION("Simple variable shift") {
        yy_scan_string("int main() {"
                       " a << 4;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Array shift") {
        yy_scan_string("int main() {"
                       " a[4] >> 'a';"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Field shift") {
        yy_scan_string("int main() {"
                       " a$field << a;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Array field Shift") {
      yy_scan_string("int main() {"
                     " a[3+2]$field_2 = 3 \% 4;"
                     "}");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests
}
