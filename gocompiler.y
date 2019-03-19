%{
    #include <stdio.h>
    int yylex(void);
    void yyerror (const char *s);

%}
//Values
%token INTLIT REALLIT STRLIT ID
//Variables
%token INT FLOAT32 STRING BOOL
//Operators
%token OR AND LT GT EQ NE LE GE 
//Ponctuation
%token PLUS MINUS STAR DIV MOD NOT
%token SEMICOLON BLANKID ASSIGN COMMA LBRACE RBRACE LPAR RPAR LSQ RSQ
//Keyword
%token PACKAGE RETURN ELSE IF FOR VAR PRINT FUNC PARSEINT CMDARGS RESERVED

%%
program 
    :   PACKAGE ID SEMICOLON declarations
    ;

declarations
    :   variable_declaration SEMICOLON
    |   function_declaration SEMICOLON
    ;


function_declaration
    :   FUNC ID LPAR RPAR function_body
    |   FUNC ID LPAR parameters RPAR function_body
    |   FUNC ID LPAR RPAR type function_body
    |   FUNC ID LPAR parameters RPAR type function_body
    ;

variable_declaration
    :   VAR variable_specification
    |   VAR LPAR variable_specification SEMICOLON RPAR
    ;

variable_specification
    :   ID id_list
    ;

id_list 
    :   type  
    |   COMMA ID id_list    
    ;

parameters  
    :   ID parameters_list
    ;

parameters_list
    :   type
    |   type COMMA ID parameters_list
    ;

type
    :   INT 
    |   FLOAT32
    |   STRING
    |   BOOL
    ;

function_body
    :
    |   LBRACE RBRACE
    |   LBRACE variables_statements RBRACE
    ;


statement
    :   ID ASSIGN expression
    ;

variables_statements
    :   variables_statements SEMICOLON
    |   variables_statements variable_declaration SEMICOLON
    |   variables_statements statement SEMICOLON
    |   variables_statements variable_declaration statement SEMICOLON
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

