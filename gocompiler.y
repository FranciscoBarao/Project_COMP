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

primal_expression
    : LPAR expression RPAR      {$$ = $2;}
    |INTLIT                     {$$ = $1;}
    |REALLIT                    {$$ = $1;}
    |ID                         {$$ = $1;}
    ;

expression 
    :   expression OR expression            {$$ = $1 | $2;}
    |   expression AND expression           {$$ = $1 && $2;}

    |   expression LT expression            {$$ = $1 < $2;}
    |   expression GT expression            {$$ = $1 > $2;}
    |   expression LE expression            {$$ = $1 <= $2;}
    |   expression GE expression            {$$ = $1 >= $2;}
    |   expression EQ expression            {$$ = $1 == $2;}
    |   expression NE expression            {$$ = $1 != $2;}

    |   MINUS expression                    {$$ = -$2;}
    |   NOT expression                      {$$ = !$2;}
    |   PLUS expression                     {$$ = +$2;}

    |   expression PLUS  math_expression          {$$ = $1 + $2;}
    |   expression MINUS math_expression         {$$ = $1 - $2;}
    |   math_expression
    ;   

math_expression
    :   math_expression STAR primal_expression   {$$ = $1 * $2;}
    |   math_expression DIV  primal_expression   {$$ = $1 / $2;}
    |   math_expression MOD  primal_expression   {$$ = $1 % $2;} 
    |   primal_expression
    ;


%%

int main() {
    yyparse();
    return 0;
}

