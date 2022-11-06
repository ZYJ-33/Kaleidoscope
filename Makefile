LLVM_LIB_PATH = /home/zhengyujia/build/lib/
LLVM_INCLUDE_PATH = /home/zhengyujia/build/include/
LLVM_PARA = `llvm-config --libs`

build:
	bison ./ast.y
	flex ./lexer.ll
	clang++ $(LLVM_PARA) -g ./ast.tab.cc ./lex.yy.cc ./driver.cc ./node.cc -o res

clean:
	rm ast.tab.cc ast.tab.hh location.hh lex.yy.cc res

