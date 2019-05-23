#Flag -g -> Gera automato em imagem (.dot)
#Flag -v -> Gera Cases Output (.output)
YACCFLAGS = -v -g 


all: compile_yacc compile_main run_test

compile_lex: Meta1/gocompiler.l lex.yy.c
		lex Meta1/gocompiler.l
		clang-3.9 -o main lex.yy.c

compile_yacc: gocompiler.l gocompiler.y
		lex gocompiler.l
		yacc -d gocompiler.y

compile_yacc_flags: gocompiler.l gocompiler.y 
		lex gocompiler.l
		yacc -d ${YACCFLAGS} gocompiler.y


compile_main: y.tab.c lex.yy.c functions.c semantics.c llvm.c
	gcc -o main y.tab.c lex.yy.c functions.c semantics.c llvm.c

make_llvm: Test.c
	clang-3.9 -S -emit-llvm Test.c


run: 
		./main -s < Test > output.ll

run_test:
		./main < Test > output.ll

run_lli:	output.ll
		clang-3.9 -o output output.s
		lli-3.9 output.ll 2

clean: 
		rm -rf *o main