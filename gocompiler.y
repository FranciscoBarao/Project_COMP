%{
    #include <stdio.h>
    #include <string.h>
    int yylex(void);
    char *yytext;
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

%%
program:  
        PACKAGE ID SEMICOLON declarations   { ;}
    ;

declarations:
        variable_declaration SEMICOLON declarations    { ;}
    |   function_declaration SEMICOLON declarations    { ;}
    |   
    ;


function_declaration:
        FUNC ID LPAR RPAR function_body                 { ;}
    |   FUNC ID LPAR parameters RPAR function_body      { ;}
    |   FUNC ID LPAR RPAR type function_body            { ;}
    |   FUNC ID LPAR parameters RPAR type function_body { ;}
    ;

variable_declaration:
        VAR variable_specification                      { ;}
    |   VAR LPAR variable_specification SEMICOLON RPAR  { ;}
    ;

variable_specification: 
        ID id_list type                                 { ;}
    ;

id_list:  
       COMMA ID id_list                                 { ;}
    |       
    ;

parameters:  
       ID type parameters_list                          { ;}
    ;

parameters_list:
        COMMA ID type parameters_list                    { ;}
    |                                                    
    ;

type:
        INT                                             { ;}
    |   FLOAT32                                         { ;} 
    |   STRING                                          { ;}
    |   BOOL                                            { ;} 
    ;       

function_body:
    |   LBRACE variables_statements RBRACE              { ;}
    ;

variables_statements:
        variables_statements SEMICOLON                          { ;}
    |   variables_statements variable_declaration SEMICOLON     { ;}
    |   variables_statements statement SEMICOLON                { ;}
    |
    ;

statement:
        assign_statement        { ;}
    |   braces_statement        { ;}
    |   if_statement            { ;}
    |   for_statement           { ;}     
    |   return_statement        { ;}
    |   union_statement         { ;}
    |   str_statement           { ;}
    |   error 
    ;

assign_statement:
        ID ASSIGN expression        { ;}
    ;

braces_statement:
        LBRACE statement_list RBRACE        { ;}
    ;
statement_list: 
        statement SEMICOLON statement_list  { ;}
    |   
    ;

if_statement:
        IF expression LBRACE statement_list RBRACE      { ;}
    |   IF expression LBRACE statement_list RBRACE ELSE LBRACE statement_list RBRACE    { ;}
    ;

for_statement:  
        FOR LBRACE statement_list RBRACE             { ;}
    |   FOR expression LBRACE statement_list RBRACE  { ;}
    ;

return_statement:
      RETURN                        { ;}
    | RETURN expression             { ;}
    ;

union_statement: 
        function_invocation         { ;}
    |   parse_arguments             { ;}
    ;

str_statement:
        PRINT LPAR expression RPAR     { ;}
    |   PRINT LPAR STRLIT RPAR          { ;}
    ;

parse_arguments:
        ID COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ expression RSQ RPAR  { ;}
    |   ID COMMA BLANKID ASSIGN PARSEINT LPAR error RPAR { ;}
    ;
function_invocation:
        ID LPAR RPAR                                 { ;}
    |   ID LPAR expression expression_list RPAR       { ;}
    |   ID LPAR error RPAR 
    ;

expression_list:
        COMMA expression expression_list                { ;}
    | 
    ;

expression:
        expression OR  expression   { ;}
    |   expression AND expression   { ;}

    |   expression LT expression    { ;}
    |   expression GT expression    { ;}
    |   expression LE expression    { ;}
    |   expression GE expression    { ;}

    |   expression EQ expression    { ;}
    |   expression NE expression    { ;}
    
    |   expression PLUS expression   {;}
    |   expression MINUS expression  {;}
    
    |   expression STAR expression  {;}
    |   expression DIV expression   {;}
    |   expression MOD expression   {;}
    
    |   NOT expression  %prec  flag {;}
    |   MINUS expression %prec flag {;}
    |   PLUS expression  %prec flag {;}
 
    |   INTLIT | REALLIT | ID   {;}
    |   LPAR expression RPAR    {;}
    |   function_invocation     { ;}
    |   LPAR error RPAR         
%%

int lex_initiate(int argc, char* argv[]);
int return_line();
int return_column();

void yyerror (const char *s) { 
    printf ("Line %d, column %d: syntax error: %s\n" ,return_line(),return_column(),yytext);
}


int main(int argc, char* argv[])
{
   lex_initiate(argc,argv);
    if(argv[1] != NULL && strcmp(argv[1] , "-l")==0) {
        yylex();
        return 0;
      } 
    yyparse();
   return 0;
}
