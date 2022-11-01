
build:
	bison ./ast.y
	flex ./lexer.ll
	g++ -g ./ast.tab.cc ./lex.yy.cc ./driver.cc -o res

clean:
	rm ast.tab.cc ast.tab.hh location.hh lex.yy.cc

