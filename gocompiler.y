%{
    #include <stdio.h>
    int yylex(void);
    void yyerror (const char *s);

%}

%token INTLIT, REALLIT,ID
//Operators & Ponctuation
%token OR,AND,LT,GT,EQ,NE,LE,GE,PLUS,MINUS,STAR,DIV,MOD,NOT,MINUS,PLUS

%%
 
calc: math_expression                        {printf("Hello World\n", $1);}

    
type
    :   INT 
    |   FLOAT32
    |   STRING
    |   BOOL
    ;
    
variable_specification
    :   ID id_list type   
    ;

id_list 
    :   COMMA ID id_list    
    ;

primal_expression
    :   LPAR math_expression RPAR      {$$ = $2;}
    |   INTLIT                         {$$ = $1;}
    |   REALLIT                        {$$ = $1;}
    |   ID                             {$$ = $1;}
    ;

math_expression 
    :   MINUS math_expression                       {$$ = -$2;}
    |   PLUS math_expression                        {$$ = +$2;}
    |   math_expression PLUS  math2_expression      {$$ = $1 + $2;}
    |   math_expression MINUS math2_expression      {$$ = $1 - $2;}
    |   math2_expression                            {$$ = $1;}
    ; 

math2_expression
    :   math2_expression STAR primal_expression   {$$ = $1 * $2;}
    |   math2_expression DIV  primal_expression   {$$ = $1 / $2;}
    |   math2_expression MOD  primal_expression   {$$ = $1 % $2;} 
    |   primal_expression
    ;

equality_expression
    :   compare_expression                          {$$ = $1;}
    |   NOT equality_expression                     {$$ = !$2;}
    |   equality_expression EQ compare_expression   {$$ = $1 == $2;}
    |   equality_expression NE compare_expression   {$$ = $1 != $2;}
    ;

compare_expression
    :   math_expression                         {$$ = $1} 
    |   compare_expression LT math_expression   {$$ = $1 < $2;}
    |   compare_expression GT math_expression   {$$ = $1 > $2;}
    |   compare_expression LE math_expression   {$$ = $1 <= $2;}
    |   compare_expression GE math_expression   {$$ = $1 >= $2;}
    ;

or_expression
    :   equality_expression                  {$$ = $1;}
    |   or_expression OR equality_expression {$$ = $1 || $2;}
    ;

and_expression
    :   equality_expression                      {$$ = $1;}
    |   and_expression AND equality_expression   {$$ = $1 || $2;}
    ;

%%

int main() {
    yyparse();
    return 0;
}

