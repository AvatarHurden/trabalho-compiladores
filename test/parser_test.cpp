#include "catch.hpp"

extern "C" {
#include "../parser.tab.h"
#include "../lex.yy.h"
#include <stdio.h>
}

struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char* str);

TEST_CASE("Empty program")
{
    yy_scan_string("");
    REQUIRE(yyparse() == 0);
}

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

    SECTION("Static Variable") {
        yy_scan_string("a static int;");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Static Array Variable") {
        yy_scan_string("a[5] static int;");
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

TEST_CASE("Command block")
{
    // Positive tests

    SECTION("Empty block") {
        yy_scan_string("int main() {"
                       " {};"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Simple block")
    {
        yy_scan_string("int main() {"
                       " {"
                       "  int a;"
                       " };"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests

    SECTION("No-semicolon block") {
        yy_scan_string("int main() {"
                       " {}"
                       "}");
        REQUIRE(yyparse() != 0);
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

    SECTION("Trailing comma") {
        yy_scan_string("int main() {"
                       " bar(a, 4, .,);"
                       "}");
        REQUIRE(yyparse() == 1);
    }
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


TEST_CASE("Exit and Return commands")
{

    // Positive tests

    SECTION("Return command") {
        yy_scan_string("int main() {"
                       " return 4 + 3;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Break command") {
        yy_scan_string("int main() {"
                       " break;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Continue command") {
        yy_scan_string("int main() {"
                       " continue;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Case command") {
      yy_scan_string("int main() {"
                     " case 3:"
                     "}");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests

    SECTION("Case with float") {
      yy_scan_string("int main() {"
                     " case 3.2:"
                     "}");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Case with semicolon") {
      yy_scan_string("int main() {"
                     " case 3;"
                     "}");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Case with expression") {
      yy_scan_string("int main() {"
                     " case 3 + 2:"
                     "}");
        REQUIRE(yyparse() == 1);
    }

    SECTION("Continue without semicolon") {
      yy_scan_string("int main() {"
                     " continue"
                     "}");
        REQUIRE(yyparse() == 1);
    }
}


TEST_CASE("Flow commands")
{

    // Positive tests

    SECTION("Simple if") {
        yy_scan_string("int main() {"
                       " if (3) then { };"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Simple if else") {
        yy_scan_string("int main() {"
                       " if (3 > 1) then { } else { };"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Compound if-else") {
        yy_scan_string("int main() {"
                       " if (\"as\") then { if (true) then { return 4; } else { }; } else { };"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Simple foreach") {
        yy_scan_string("int main() {"
                       " foreach ( a: 3) { output a; };"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Compound foreach") {
        yy_scan_string("int main() {"
                       " foreach ( a: 3, 4, 'a') { output a; };"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Simple for") {
        yy_scan_string("int main() {"
                       " for (i = 0: i < 3: i = i + 1) { output a; };"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("For with list in first") {
        yy_scan_string("int main() {"
                       " for (i = 0, j = 1: i < 3: i = i + 1) { output a; };"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("For with list in second") {
        yy_scan_string("int main() {"
                       " for (i = 0: i < 3: i = i + 1, break) { output a; };"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("while") {
        yy_scan_string("int main() {"
                       " while (i > 3) do { case 3: };"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("do while") {
        yy_scan_string("int main() {"
                       " do { input \"test\"; } while (i > 3);"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("switch") {
        yy_scan_string("int main() {"
                       " switch (3+4) { case 7: continue; case 1: break;};"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests

    SECTION("If without semicolon") {
        yy_scan_string("int main() {"
                       " if (true) then { } else { }"
                       "}");
        REQUIRE(yyparse() == 1);
    }

    SECTION("foreach trailing comma") {
        yy_scan_string("int main() {"
                       " foreach ( a: 3, 4, 'a', ) { output a; };"
                       "}");
        REQUIRE(yyparse() == 1);
    }

    SECTION("foreach empty") {
        yy_scan_string("int main() {"
                       " foreach ( a: ) { output a; };"
                       "}");
        REQUIRE(yyparse() == 1);
    }

    SECTION("for empty first list") {
        yy_scan_string("int main() {"
                       " for (: i < 3: i + 1) { output a; };"
                       "}");
        REQUIRE(yyparse() == 1);
    }

    SECTION("for empty second list") {
        yy_scan_string("int main() {"
                       " for (i = 0: i < 3:) { output a; };"
                       "}");
        REQUIRE(yyparse() == 1);
    }

    SECTION("for with output command") {
        yy_scan_string("int main() {"
                       " for (output 3: i < 3: i + 1) { output a; };"
                       "}");
        REQUIRE(yyparse() == 1);
    }

    SECTION("for with case command") {
        yy_scan_string("int main() {"
                       " for (case 3: i < 3: i + 1) { output a; };"
                       "}");
        REQUIRE(yyparse() == 1);
    }

    SECTION("while with list") {
        yy_scan_string("int main() {"
                       " while (i > 3, j < 3) do { break; };"
                       "}");
        REQUIRE(yyparse() == 1);
    }
}


TEST_CASE("Pipe Commands")
{

    // Positive tests

    SECTION("Bash pipe") {
        yy_scan_string("int main() {"
                       " f() \%|\% g(a);"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Forward pipe") {
        yy_scan_string("int main() {"
                       " f(.) \%>\% g(4, .);"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Compound pipe") {
        yy_scan_string("int main() {"
                       " f() \%|\% g() \%>\% foo(., .);"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests

    SECTION("Missing second argument") {
        yy_scan_string("int main() {"
                       " f() \%|\%;"
                       "}");
        REQUIRE(yyparse() == 1);
    }

    SECTION("With non-function call") {
        yy_scan_string("int main() {"
                       " 4 \%|\% f();"
                       "}");
        REQUIRE(yyparse() == 1);
    }
}


TEST_CASE("Expression Parts")
{

    // Positive tests

    SECTION("Int as expressions") {
        yy_scan_string("int main() {"
                       " a = 4;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Float as expression") {
        yy_scan_string("int main() {"
                       " a = 4.2;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Char as expression") {
        yy_scan_string("int main() {"
                       " a = 'c';"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("String as expression") {
        yy_scan_string("int main() {"
                       " a = \"test\";"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("True as expression") {
        yy_scan_string("int main() {"
                       " a = true;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("False as expression") {
        yy_scan_string("int main() {"
                       " a = false;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Pipe as expression") {
        yy_scan_string("int main() {"
                       " a = f(.) \%>\% g(4, .);"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Variable as expression") {
        yy_scan_string("int main() {"
                       " a = b;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Variable array as expression") {
        yy_scan_string("int main() {"
                       " a = b[3 + 2];"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Variable field as expression") {
        yy_scan_string("int main() {"
                       " a = b$field;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Variable array field as expression") {
        yy_scan_string("int main() {"
                       " a = b[3]$field;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Parenthesis expression expression") {
        yy_scan_string("int main() {"
                       " a = (3 + 4);"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests
}


TEST_CASE("Compound Expressions")
{

    // Positive tests

    SECTION("Negative Int") {
        yy_scan_string("int main() {"
                       " a = -4;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Positive Int") {
        yy_scan_string("int main() {"
                       " a = +4;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Negative Float") {
        yy_scan_string("int main() {"
                       " a = -4.2;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Positive Float") {
        yy_scan_string("int main() {"
                       " a = +4.2;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Sum of Negatives") {
        yy_scan_string("int main() {"
                       " a = -4 + -4.2;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Subtraction of Negatives") {
        yy_scan_string("int main() {"
                       " a = -4 - -4.2;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Greater or equal operator") {
        yy_scan_string("int main() {"
                       " a = 4 >= 3;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Less or equal operator") {
        yy_scan_string("int main() {"
                       " a = 4 <= 3;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("equal operator") {
        yy_scan_string("int main() {"
                       " a = 4 == (3 + 2);"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("not equal operator") {
        yy_scan_string("int main() {"
                       " a = true != 3 - 2;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("and operator") {
        yy_scan_string("int main() {"
                       " a = true && false;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("or operator") {
        yy_scan_string("int main() {"
                       " a = 4 || 3;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("prefix pointer operator") {
        yy_scan_string("int main() {"
                       " a = *b;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("prefix address operator") {
        yy_scan_string("int main() {"
                       " a = &b;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("prefix not operator") {
        yy_scan_string("int main() {"
                       " a = !b;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("prefix and infix operators") {
        yy_scan_string("int main() {"
                       " a = !b + +2 - -3;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests
}


TEST_CASE("Ternary Expression")
{

    // Positive tests

    SECTION("Simple") {
        yy_scan_string("int main() {"
                       " a = t ? 4 : 2;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Compound then") {
        yy_scan_string("int main() {"
                       " a = t ? 4 + 2 : 4;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Compound then else") {
        yy_scan_string("int main() {"
                       " a = t ? -5 * +a : f() \%|\% g(.);"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Compound condition") {
        yy_scan_string("int main() {"
                       " a = a*2 > 5 ? 4 : 1;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    SECTION("Nested ternary") {
        yy_scan_string("int main() {"
                       " a = a > 2 ? t ? 5 : 1 : 6;"
                       "}");
        REQUIRE(yyparse() == 0);
    }

    // Negative tests

    SECTION("Nested ternary without closing") {
        yy_scan_string("int main() {"
                       " a = a > 2 ? t ? 5 : 1;"
                       "}");
        REQUIRE(yyparse() == 1);
    }
}
