all: parser.y lex.l main.c symTable.c arrayTable.c quad.c list.c def.h
	yacc -d parser.y
	lex lex.l
	gcc y.tab.c lex.yy.c symTable.c arrayTable.c main.c quad.c list.c
run: all
	./a.out input.txt
clean:
	-rm -rf a.out lex.yy.c y.tab.c y.tab.h