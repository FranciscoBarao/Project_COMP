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

expression 
    :   LPAR expression RPAR                {$$ = $2;}
    
    |   expression OR expression            {$$ = $1 | $2;}
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


    |   expression PLUS expression          {$$ = $1 + $2;}
    |   expression MINUS expression         {$$ = $1 - $2;}
    |   expression STAR expression          {$$ = $1 * $2;}
    |   expression DIV expression           {$$ = $1 / $2;}
    |   expression MOD expression           {$$ = $1 % $2;}

    |   INTLIT | REALLIT | ID               {$$ = $1;}         
    ;   
%%

int main() {
    yyparse();
    return 0;
}

