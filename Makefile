# Compiler options
CC := gcc
CPPC := g++ -std=c++11
CFLAGS = -g -Wall

# Project dirs
TEST_DIR := test

# Executables
TEST_EXE := $(TEST_DIR)/run_tests

# Sources
TEST_SRC_FILES := catch.cpp parser_test.cpp scanner_test.cpp
TEST_SRCS := $(addprefix $(TEST_DIR)/, $(TEST_SRC_FILES))

# Objects
TEST_OBJ_FILES := $(TEST_SRC_FILES:%.cpp=%.o)
TEST_OBJS := $(addprefix $(TEST_DIR)/, $(TEST_OBJ_FILES))

# Variables
etapa=2

# Rules
all: lex.yy.o
	@echo "\n - Link parser"
	$(CC) $(CFLAGS) main.c lex.yy.o parser.tab.o -lfl -o etapa$(etapa)
	@echo " - Done!"

lex.yy.o: parser.y scanner.l
	@echo "\n - Compile parser"
	bison -d parser.y -Wall --verbose
	flex --header-file=lex.yy.h scanner.l
	$(CC) -c lex.yy.c parser.tab.c

test: lex.yy.o $(TEST_OBJS)
	@echo "\n - Link tests"
	$(CPPC) parser.tab.o lex.yy.o $(TEST_OBJS) -lfl -o test/run_tests
	@echo "\n - Run tests"
	./$(TEST_DIR)/run_tests

$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp
	@echo "\n - Compile $<";
	$(CPPC) -c $< -o $@

zip:
	tar cvzf etapa$(etapa).tgz Makefile main.c scanner.l parser.y

clean:
	rm -f etapa* lex.yy.* parser.tab.* *.o test/scanner_test.o test/parser_test.o $(TEST_EXE)
