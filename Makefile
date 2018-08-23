all:
	flex scanner.l
	gcc -c lex.yy.c
	gcc -Wall main.c lex.yy.o -lfl -o etapaX

zip:
	tar cvzf etapaX.tgz Makefile main.c tokens.h scanner.l

clean:
	rm -f main lex.yy.*
