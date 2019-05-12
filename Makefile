#Flag -g -> Gera automato em imagem (.dot)
#Flag -v -> Gera Cases Output (.output)
YACCFLAGS = -v -g 


all: compile_yacc compile_main 

compile_lex: Meta1/gocompiler.l lex.yy.c
		lex Meta1/gocompiler.l
		clang-3.9 -o main lex.yy.c

compile_yacc: gocompiler.l gocompiler.y 
		lex gocompiler.l
		yacc -d gocompiler.y

compile_yacc_flags: gocompiler.l gocompiler.y 
		lex gocompiler.l
		yacc -d ${YACCFLAGS} gocompiler.y


compile_main: y.tab.c lex.yy.c functions.c semantics.c
	gcc -o main y.tab.c lex.yy.c functions.c semantics.c

run: 
		./main -l
run_test:
		./main -t < Test
clean: 
		rm -rf *o main