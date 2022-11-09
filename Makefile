LLVM_LIB_PATH = /home/zhengyujia/build/lib/
LLVM_INCLUDE_PATH = /home/zhengyujia/build/include/
LLVM_PARA = `llvm-config --libs`

runtime:
	g++ ./runtime.cc --shared -fPIC -o runtime.so

build: runtime
	bison ./ast.y
	flex ./lexer.ll
	clang++ $(LLVM_PARA) -g ./ast.tab.cc ./lex.yy.cc ./driver.cc ./runtime.so  -o res

clean:
	rm ast.tab.cc ast.tab.hh location.hh lex.yy.cc res

