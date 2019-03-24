%{
    #include <stdio.h>
    int yylex(void);
    void yyerror (const char *s);

%}
//Values
%token INTLIT REALLIT Strlit ID
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
 program: 
        PACKAGE ID SEMICOLON declarations               {printf("program\n");}
    ;

declarations:
        variable_declaration SEMICOLON                  {printf("declaration\n");}
    | 
    ;

variable_declaration:
        VAR variable_specification                           {printf("variable declaration 1\n");}
    |   VAR LPAR variable_specification SEMICOLON RPAR       {printf("variable declaration 2\n");} 
    ;   


variable_specification: 
        ID id_list type         {printf("variable specification\n");}
    ;

id_list:  
       COMMA ID id_list         {printf("id_list\n");}
    |       
    ;


type:
        INT                      {printf("TERMINAL - INT\n");}                          
    |   FLOAT32                  {printf("TERMINAL - FLOAT32\n");}                      
    |   STRING                   {printf("TERMINAL - STRING\n");}                       
    |   BOOL                     {printf("TERMINAL - BOOL\n");}                      
    ;
%%


/*  
%%
 program: 
        PACKAGE ID SEMICOLON declarations
    ;

declarations:
        variable_declaration SEMICOLON declarations
    |   function_declaration SEMICOLON declarations
    |   
    ;


function_declaration:
        FUNC ID LPAR RPAR function_body
    |   FUNC ID LPAR parameters RPAR function_body
    |   FUNC ID LPAR RPAR type function_body
    |   FUNC ID LPAR parameters RPAR type function_body
    ;

variable_declaration:
        VAR variable_specification
    |   VAR LPAR variable_specification SEMICOLON RPAR
    ;

variable_specification: 
        ID id_list type
    ;

id_list:  
       COMMA ID id_list
    |       
    ;

parameters:  
       ID type parameters_list
    ;

parameters_list:
        COMMA ID type parameters_list
    |   
    ;

type:
        INT 
    |   FLOAT32
    |   STRING
    |   BOOL
    ;

function_body:
    |   LBRACE variables_statements RBRACE
    ;

variables_statements:
        variables_statements SEMICOLON
    |   variables_statements variable_declaration SEMICOLON
    |   variables_statements statement SEMICOLON
    |   
    ;



statement:
        assign_statement   
    |   braces_statement
    |   if_statement
    |   for_statement
    |   return_statement
    |   union_statement 
    |   str_statement
    ;

assign_statement:
        ID ASSIGN expression
    ;

braces_statement:
        LBRACE statement_list RBRACE
    ;
statement_list: 
        statement SEMICOLON statement_list
    |   
    ;

if_statement:
        IF expression LBRACE statement_list RBRACE
    |   IF expression LBRACE statement_list RBRACE ELSE LBRACE statement_list RBRACE
    ;

for_statement:  
        FOR LBRACE statement_list RBRACE    
    |   FOR expression LBRACE statement_list RBRACE
    ;

return_statement:
      RETURN 
    | RETURN expression
    ;

union_statement: 
        function_invocation
    |   parse_arguments
    ;

str_statement:
        PRINT LPAR expression RPAR   
    |   PRINT LPAR Strlit RPAR
    ;

parse_arguments:
        ID COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ expression RSQ RPAR
    ;
function_invocation:
        ID LPAR RPAR
    |   ID LPAR expression expression_list RPAR
    ;

expression_list:
        COMMA expression expression_list
    | 
    ;



expression:
        or_and_expression
    ;


primal_expression:
        LPAR math_expression RPAR      {$$ = $2;}
    |   INTLIT                         {$$ = $1;}
    |   REALLIT                        {$$ = $1;}
    |   ID                             {$$ = $1;}
    ;


math2_expression:
        math2_expression STAR primal_expression   {$$ = $1 * $2;}
    |   math2_expression DIV  primal_expression   {$$ = $1 / $2;}
    |   math2_expression MOD  primal_expression   {$$ = $1 % $2;}
    |   primal_expression
    ;

math_expression: 
        MINUS math_expression                       {$$ = -$2;}
    |   PLUS math_expression                        {$$ = +$2;}
    |   math_expression PLUS  math2_expression      {$$ = $1 + $2;}
    |   math_expression MINUS math2_expression      {$$ = $1 - $2;}
    |   math2_expression                            {$$ = $1;}
    ; 


compare_expression:
        math_expression                         {$$ = $1} 
    |   compare_expression LT math_expression   {$$ = $1 < $2;}
    |   compare_expression GT math_expression   {$$ = $1 > $2;}
    |   compare_expression LE math_expression   {$$ = $1 <= $2;}
    |   compare_expression GE math_expression   {$$ = $1 >= $2;}
    ;

equality_expression:
        compare_expression                          {$$ = $1;}
    |   NOT equality_expression                     {$$ = !$2;}
    |   equality_expression EQ compare_expression   {$$ = $1 == $2;}
    |   equality_expression NE compare_expression   {$$ = $1 != $2;}
    ;

or_and_expression:
        equality_expression                         {$$ = $1;}
    |   or_and_expression OR equality_expression    {$$ = $1 || $2;}
    |   or_and_expression AND equality_expression   {$$ = $1 && $2;}
    ;
%%  

*/