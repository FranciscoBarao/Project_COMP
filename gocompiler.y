%{
    #include <stdio.h>
    int yylex(void);
    void yyerror (const char *s);

%}
%define parse.error verbose
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

%token flag

%left OR AND
%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left DIV MOD STAR
%left LPAR
%right flag

/*
expression:
        or_and_expression                                {printf("expression\n");}
    ;


primal_expression:  
        LPAR math_expression RPAR       {printf("LPAR math RPAR\n");}
    |   INTLIT                          {printf("INTLIT\n");}
    |   REALLIT                         {printf("REALIT\n");}
    |   ID                              {printf("ID\n");}
    ;


math2_expression:
        math2_expression STAR primal_expression   {printf("math2 STAR\n");}
    |   math2_expression DIV  primal_expression   {printf("math2 DIV\n");}
    |   math2_expression MOD  primal_expression   {printf("math2 MOD\n");}
    |   primal_expression                         {printf("math2 primal\n");}
    ;

math_expression: 
        MINUS math_expression                       {printf("MINUS math\n");}
    |   PLUS math_expression                        {printf("PLUS math\n");}
    |   math2_expression PLUS  math_expression      {printf("math PLUS\n");}
    |   math2_expression MINUS math_expression      {printf("math MINUS\n");}
    |   math2_expression                            {printf("math 2\n");}
    ; 


compare_expression:
        math_expression                         {printf("compare math expression\n");}
    |   compare_expression LT math_expression   {printf("compare LT\n");}
    |   compare_expression GT math_expression   {printf("compare GT\n");}
    |   compare_expression LE math_expression   {printf("compare LE\n");}
    |   compare_expression GE math_expression   {printf("compare GE\n");}
    ;

equality_expression:
        compare_expression                          {printf("equality compare\n");}
    |   NOT equality_expression                     {printf("NOT equality\n");}
    |   compare_expression EQ equality_expression   {printf("EQ equality expression\n");} /*TROQUEI COMPARE E EQUALITY PARA ARVORE DESCER SEMPRE PELA DIREITA... WORKS?!?!?!
    |   compare_expression NE equality_expression   {printf("NE equality expression\n");}
    ;

or_and_expression:
        equality_expression                       {printf("OR AND equality expression\n");}
    |   or_and_expression OR equality_expression  {printf("OR expression\n");}
    |   or_and_expression AND equality_expression {printf("AND expression\n");}
    ;
*/
%%
program:  
        PACKAGE ID SEMICOLON declarations   {printf("program\n");}
    ;

declarations:
        variable_declaration SEMICOLON declarations    {printf("declaration1\n");}
    |   function_declaration SEMICOLON declarations    {printf("declaration2\n");}
    |   
    ;


function_declaration:
        FUNC ID LPAR RPAR function_body                 {printf("function declaration1\n");}
    |   FUNC ID LPAR parameters RPAR function_body      {printf("function declaration2\n");}
    |   FUNC ID LPAR RPAR type function_body            {printf("function declaration3\n");}
    |   FUNC ID LPAR parameters RPAR type function_body {printf("function declaration4\n");}
    ;

variable_declaration:
        VAR variable_specification                      {printf("Variable declaration1\n");}
    |   VAR LPAR variable_specification SEMICOLON RPAR  {printf("Variable declaration2\n");}
    ;

variable_specification: 
        ID id_list type                                 {printf("Variable specification\n");}
    ;

id_list:  
       COMMA ID id_list                                 {printf("id list \n");}
    |       
    ;

parameters:  
       ID type parameters_list                          {printf("Parameter\n");}
    ;

parameters_list:
        COMMA ID type parameters_list                    {printf("Parameter List\n");}
    |                                                    
    ;

type:
        INT                                             {printf("INT\n");}
    |   FLOAT32                                         {printf("FLOAT32\n");} 
    |   STRING                                          {printf("STRING\n");}
    |   BOOL                                            {printf("BOOL\n");} 
    ;       

function_body:
    |   LBRACE variables_statements RBRACE              {printf("Function body\n");}
    ;

variables_statements:
        variables_statements SEMICOLON                          {printf("Variable statement1\n");}
    |   variables_statements variable_declaration SEMICOLON     {printf("Variable statement2\n");}
    |   variables_statements statement SEMICOLON                {printf("Variable statement3\n");}
    |
    ;

statement:
        assign_statement        {printf("Statement1\n");}
    |   braces_statement        {printf("Statement2\n");}
    |   if_statement            {printf("Statement3\n");}
    |   for_statement           {printf("Statement4\n");}     
    |   return_statement        {printf("Statement5\n");}
    |   union_statement         {printf("Statement6\n");}
    |   str_statement           {printf("Statement7\n");}
    ;

assign_statement:
        ID ASSIGN expression        {printf("Assign Statement\n");}
    ;

braces_statement:
        LBRACE statement_list RBRACE        {printf("Braces Statement\n");}
    ;
statement_list: 
        statement SEMICOLON statement_list  {printf("statement List\n");}
    |   
    ;

if_statement:
        IF expression LBRACE statement_list RBRACE      {printf("if statement1\n");}
    |   IF expression LBRACE statement_list RBRACE ELSE LBRACE statement_list RBRACE    {printf("if statement2\n");}
    ;

for_statement:  
        FOR LBRACE statement_list RBRACE             {printf("for statement1\n");}
    |   FOR expression LBRACE statement_list RBRACE  {printf("for statement2\n");}
    ;

return_statement:
      RETURN                        {printf("Return1\n");}
    | RETURN expression             {printf("Return2\n");}
    ;

union_statement: 
        function_invocation         {printf("Union Statement1\n");}
    |   parse_arguments             {printf("Union Statement2\n");}
    ;

str_statement:
        PRINT LPAR expression RPAR     {printf("STR statement1\n");}
    |   PRINT LPAR STRLIT RPAR          {printf("STR statement2\n");}
    ;

parse_arguments:
        ID COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ expression RSQ RPAR  {printf("Parse arguments\n");}
    ;
function_invocation:
        ID LPAR RPAR                                 {printf("function invocation1\n");}
    |   ID LPAR expression expression_list RPAR       {printf("function invocation2\n");}
    ;

expression_list:
        COMMA expression expression_list                {printf("expression list\n");}
    | 
    ;

expression:
        expression OR  expression   {printf("expr 1\n");}
    |   expression AND expression   {printf("expr 2\n");}

    |   expression LT expression    {printf("expr 3\n");}
    |   expression GT expression    {printf("expr 4\n");}
    |   expression LE expression    {printf("expr 5\n");}
    |   expression GE expression    {printf("expr 6\n");}

    |   expression EQ expression    {printf("expr 7\n");}
    |   expression NE expression    {printf("expr 8\n");}
    
    |   expression PLUS expression   {$$ = $1 + $3; printf("expr 9\n");}
    |   expression MINUS expression  {$$ = $1 - $3; printf("expr 10\n");}
    
    |   expression STAR expression  {$$ = $1 * $3; printf("expr 11\n");}
    |   expression DIV expression   {$$ = $1 / $3; printf("expr 12\n");}
    |   expression MOD expression   {$$ = $1 % $3; printf("expr 13\n");}
    
    |   MINUS expression %prec flag {$$ = -$2; printf("expr 14\n");}
    |   PLUS expression  %prec flag {$$ = +$2; printf("expr 15\n");}
 
    |   INTLIT | REALLIT | ID   {$$=$1; printf("expr 16\n");}
    |   LPAR expression RPAR    {$$=$2; printf("expr 17\n");}
    |   function_invocation     {printf("expr 18\n");}
%%


void yyerror (const char *s) { 
    printf ("s-> %s \n",s);
}

int lex_initiate(int argc, char* argv[]);

int main(int argc, char* argv[])
{
   lex_initiate(argc,argv);
   yyparse();
   return 0;
}
