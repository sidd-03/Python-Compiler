m1.out: parser.tab.c lex.yy.c 
	g++ -std=c++11 parser.tab.c lex.yy.c -o m1.out
parser.tab.c: parser.y
	bison -tdv parser.y
lex.yy.c: lexer.l parser.tab.h
	flex lexer.l
clean: 
	rm -f m1.out parser.tab.c lex.yy.c parser.tab.h parser.output