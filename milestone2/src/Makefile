m2.out: parser.cpp lexer.cpp
	g++ -std=c++11 make_tac.cpp ast.cpp lexer.cpp parser.cpp -o m2.out

parser.cpp: parser.y
	bison -d -o $@ $<

lexer.cpp: lexer.l
	flex -o $@ $<

clean:
	rm -f parser.cpp parser.hpp lexer.cpp m2.out