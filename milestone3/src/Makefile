m3.out: parser.cpp lexer.cpp
	g++ -std=c++11 make_x86.cpp make_tac.cpp ast.cpp lexer.cpp parser.cpp -o m3.out

parser.cpp: parser.y
	bison -d -o $@ $<

lexer.cpp: lexer.l
	flex -o $@ $<

clean:
	rm -f dump.txt parser.cpp parser.hpp lexer.cpp m3.out