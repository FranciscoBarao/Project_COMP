%{
    #include <stdio.h>
    int yylex(void);
    void yyerror (const char *s);

%}

%token INTLIT, REALLIT,ID
//Operators & Ponctuation
%token OR,AND,LT,GT,EQ,NE,LE,GE,PLUS,MINUS,STAR,DIV,MOD,NOT,MINUS,PLUS
%%
 
calc: expression                        {printf("Hello World\n", $1);}

expression: LPAR expression RPAR                                       {$$ = $2;}
    |   expression (OR | AND) expression                               {$$=}
    |   expression (LT | GT | EQ | NE | LE | GE) expression            {$$=}
    |   expression (PLUS | MINUS | STAR | DIV | MOD) expression        {$$=}
    |   (NOT | MINUS | PLUS) expression                                 {} 
    |   INTLIT | REALLIT | ID
    ;
%%

int main() {
    yyparse();
    return 0;
}

