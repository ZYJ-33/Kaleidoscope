LLVM_LIB_PATH = /home/zhengyujia/build/lib/
LLVM_INCLUDE_PATH = /home/zhengyujia/build/include/
LLVM_PARA = `llvm-config --libs`

runtime:
	clang++ ./runtime.cc -g --shared -fPIC -o runtime.so

bison:
	bison ./ast.y

flex:
	flex ./lexer.ll

node.o:
	clang++ -c -g ./node.cc -o node.o

ast.o: 
	clang++ -c -g ./ast.tab.cc -o ast.o

lex.o:
	clang++ -c -g ./lex.yy.cc -o lex.o

driver.o:
	clang++ -c -g ./driver.cc -o driver.o

build:flex bison runtime ast.o lex.o driver.o node.o
	clang++ $(LLVM_PARA) -g ./node.o ./ast.o ./lex.o ./driver.o ./runtime.so  -o res

clean:
	rm ast.tab.cc ast.tab.hh location.hh lex.yy.cc runtime.so res node.o ast.o lex.o driver.o

