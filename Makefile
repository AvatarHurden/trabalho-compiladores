etapa=1

all:
	flex --header-file=lex.yy.h scanner.l
	gcc -c lex.yy.c
	gcc -Wall main.c lex.yy.o -lfl -o etapa$(etapa)

zip:
	tar cvzf etapa$(etapa).tgz Makefile main.c tokens.h scanner.l

test:
	g++ -std=c++11 test/main_test.cpp -c
	g++ -std=c++11 test/main_test.o test/scanner_test.cpp -o test/run_tests

clean:
	rm -f etapa* lex.yy.*
