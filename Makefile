etapa=1

all:
	flex scanner.l
	gcc -c lex.yy.c
	gcc -Wall main.c lex.yy.o -lfl -o etapa$(etapa)

zip:
	tar cvzf etapa$(etapa).tgz Makefile main.c tokens.h scanner.l

clean:
	rm -f etapa* lex.yy.*
