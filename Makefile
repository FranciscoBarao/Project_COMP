#All compiles everything

all: compile run

compile: gocompiler.l lex.yy.c
		lex gocompiler.l
		clang-3.9 -o main lex.yy.c
run:
		./main
run_test:
		./main < Test
clean: 
		rm -rf *o main