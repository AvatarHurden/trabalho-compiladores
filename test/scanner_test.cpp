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
}
