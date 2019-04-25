%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "functions.h"
    #include "y.tab.h"

    int yylex(void);
    extern char *yytext;
    void yyerror (const char *s);
    Structure* copy_type(Structure* type, Structure* brother);

    int is_error = 0;
    Structure* myprogram;
%}
%define parse.error verbose
//Values
%token<value> INTLIT REALLIT STRLIT ID
//Variables
%token<node> INT FLOAT32 STRING BOOL
//Operators
%token<node> OR AND LT GT EQ NE LE GE 
//Ponctuation
%token<node> PLUS MINUS STAR DIV MOD NOT
%token<node> SEMICOLON BLANKID ASSIGN COMMA LBRACE RBRACE LPAR RPAR LSQ RSQ
//Keyword
%token <node>PACKAGE RETURN ELSE IF FOR VAR PRINT FUNC PARSEINT CMDARGS RESERVED
%token flag

%type<node>program
%type<node>declarations

%type<node>function_declaration
%type<node>variable_declaration
%type<node>variable_specification
%type<node>id_list
%type<node>parameters
%type<node>parameters_list
%type<node>type
%type<node>function_body
%type<node>variables_statements
%type<node>statement
%type<node>assign_statement
%type<node>braces_statement
%type<node>statement_list
%type<node>if_statement
%type<node>for_statement
%type<node>return_statement
%type<node>union_statement
%type<node>str_statement
%type<node>parse_arguments
%type<node>function_invocation
%type<node>expression_list
%type<node>expression
%type<node>error
%type<node>id_state
%type<node>parameters_decl
%type<node>function_header
%type<node>strlit_state
%type<node>block_statement
%type<node>block_if_childs

%union{
    Token value;
    Structure *node;
}

%left OR 
%left AND
%left EQ NE LT GT LE GE
%left PLUS MINUS
%left DIV MOD STAR
%left LPAR
%right flag /* left flag \n left LPAR LBRACE LSQ \n rigth RPAR RBRACE RSQ */

%%
program: 
        PACKAGE ID SEMICOLON declarations                      {$$=myprogram=create_node("Program", 0, 0, Program, $4);}
    ;

declarations:
        variable_declaration SEMICOLON declarations            {$$=$1; add_brother($1, $3);}
    |   function_declaration SEMICOLON declarations            {$$=$1; add_brother($1, $3);}
    |                                                          {$$=(Structure*)NULL;}
    ;


function_declaration:
        function_header function_body                          {$$=create_node("FuncDecl", 0, 0, FuncDecl, $1); add_brother($1, $2);}
    ;

function_header:
        FUNC id_state LPAR parameters RPAR                     {$$=create_node("FuncHeader", 0, 0, FuncHeader, $2); add_brother($2, $4);}
    |   FUNC id_state LPAR parameters RPAR type                {$$=create_node("FuncHeader", 0, 0, FuncHeader, $2); add_brother($2, $6); add_brother($6, $4);}
    ;

variable_declaration:
        VAR variable_specification type                        {$$=$2; Structure* tmp = $2; while(tmp != NULL){ tmp->child = copy_type($3, tmp->child); tmp = tmp->brother; }}
    |   VAR LPAR variable_specification type SEMICOLON RPAR    {$$=$3; Structure* tmp = $3; while(tmp != NULL){ tmp->child = copy_type($4, tmp->child); tmp = tmp->brother; }}
    ;

variable_specification:
        id_state id_list                                       {Structure* vardecl = create_node("VarDecl", 0, 0, VarDecl, $1); add_brother(vardecl, $2); $$=vardecl;}
    ;

id_list:  
        COMMA id_state id_list                                 {Structure* vardecl = create_node("VarDecl", 0, 0, VarDecl, $2); add_brother(vardecl, $3); $$=vardecl;}
    |                                                          {$$=(Structure*)NULL;}
    ;

id_state:
        ID                                                     {$$=create_node($1.val,$1.col,$1.l,id, NULL);}
    ;

parameters:  
        parameters_decl                                        {$$=create_node("FuncParams",0,0,FuncParams, $1);}
    ;

parameters_decl:
        id_state type parameters_list                          {Structure* paramsdecl = create_node("ParamDecl",0,0,ParamDecl, $2); add_brother($2, $1); add_brother(paramsdecl, $3); $$=paramsdecl;}
    |                                                          {$$=(Structure*)NULL;}
    ;


parameters_list:
        COMMA id_state type parameters_list                    {Structure* paramsdecl = create_node("ParamDecl",0,0,ParamDecl, $3); add_brother($3, $2); add_brother(paramsdecl, $4); $$=paramsdecl;}
    |                                                          {$$=(Structure*)NULL;}
    ;

type:
        INT                                                    {$$=create_node("Int",0,0,type, NULL);}
    |   FLOAT32                                                {$$=create_node("Float32",0,0,type, NULL);} 
    |   STRING                                                 {$$=create_node("String",0,0,type, NULL);}
    |   BOOL                                                   {$$=create_node("Bool",0,0,type, NULL);} 
    ;       

function_body:                                                 {$$=(Structure*)NULL;}
    |   LBRACE variables_statements RBRACE                     {$$=create_node("FuncBody",0,0,FuncBody, $2);}
    ;

variables_statements:
        variables_statements SEMICOLON                         {$$=$1;}
    |   variables_statements variable_declaration SEMICOLON    {if($1 == NULL){$$=$2;} else{ $$=$1; add_brother($1, $2);}}
    |   variables_statements statement SEMICOLON               {if($1 == NULL){$$=$2;} else{ $$=$1; add_brother($1, $2);}}
    |                                                          {$$=(Structure*)NULL;}
    ;

statement:
        assign_statement                                       {$$=$1;}
    |   braces_statement                                       {$$=$1;}
    |   if_statement                                           {$$=$1;}
    |   for_statement                                          {$$=$1;}     
    |   return_statement                                       {$$=$1;}
    |   union_statement                                        {$$=$1;}
    |   str_statement                                          {$$=$1;}
    |   error                                                  {$$=(Structure*)NULL; is_error=1;}
    ;

assign_statement:
        id_state ASSIGN expression                             {$$=create_node("Assign",0,0,Statement, $1); add_brother($1, $3);}
    ;

braces_statement:
        LBRACE block_if_childs RBRACE                          {$$=$2;}
    ;    

block_if_childs:
        statement_list                                         {if($1 != NULL && $1->brother != NULL){ $$ = create_node("Block",0,0,Block, $1); }else{$$=$1;}}
    ;

block_statement:
        statement_list                                         {$$ = create_node("Block",0,0,Block, $1);}
    ;

statement_list: 
        statement SEMICOLON statement_list                     {if($1!=NULL){$$=$1; add_brother($1, $3);}else{$$=$3;}}
    |                                                          {$$=(Structure*)NULL;}
    ;

if_statement:
        IF expression LBRACE block_statement RBRACE block_statement                       {$$=create_node("If",0,0,Statement, $2); add_brother($2, $4); add_brother($4, $6);}
    |   IF expression LBRACE block_statement RBRACE ELSE LBRACE block_statement RBRACE    {$$=create_node("If",0,0,Statement, $2); add_brother($2, $4); add_brother($4, $8);}
    ;

for_statement:  
        FOR LBRACE block_statement RBRACE                        {$$=create_node("For",0,0,Statement, $3);}
    |   FOR expression LBRACE block_statement RBRACE             {$$=create_node("For",0,0,Statement, $2); add_brother($2, $4);}
    ;

return_statement:
        RETURN                                                  {$$=create_node("Return",0,0,Statement, NULL);}
    |   RETURN expression                                       {$$=create_node("Return",0,0,Statement, $2);}
    ;

union_statement: 
        function_invocation                                     {$$=$1;}
    |   parse_arguments                                         {$$=$1;}
    ;

str_statement:
        PRINT LPAR expression RPAR                              {$$=create_node("Print",0,0,Statement, $3);}
    |   PRINT LPAR strlit_state RPAR                            {$$=create_node("Print",0,0,Statement, $3);}
    ;

strlit_state:
        STRLIT                                                  {$$=create_node($1.val,$1.col,$1.l,strlit, NULL);}
    ;

parse_arguments:
        id_state COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ expression RSQ RPAR     {$$=create_node("ParseArgs",0,0,ParseArgs, $1); add_brother($1, $9);}
    |   id_state COMMA BLANKID ASSIGN PARSEINT LPAR error RPAR                          {$$=(Structure*)NULL; is_error=1;}
    ;
function_invocation:
        id_state LPAR RPAR                                      {$$=create_node("Call",0,0,Call, $1);}
    |   id_state LPAR expression expression_list RPAR           {$$=create_node("Call",0,0,Call, $1); add_brother($1, $3); add_brother($3, $4);}
    |   id_state LPAR error RPAR                                {$$=(Structure*)NULL; is_error=1;}
    ;

expression_list:
        COMMA expression expression_list                        {$$=$2; add_brother($2,$3);}
    |                                                           {$$=(Structure*)NULL;}
    ;

expression:
        expression OR  expression                               {$$=create_node("Or",0,0,Expression, $1); add_brother($1, $3);}
    |   expression AND expression                               {$$=create_node("And",0,0,Expression, $1); add_brother($1, $3);}
    |   expression LT expression                                {$$=create_node("Lt",0,0,Expression, $1); add_brother($1, $3);}
    |   expression GT expression                                {$$=create_node("Gt",0,0,Expression, $1); add_brother($1, $3);}
    |   expression LE expression                                {$$=create_node("Le",0,0,Expression, $1); add_brother($1, $3);}
    |   expression GE expression                                {$$=create_node("Ge",0,0,Expression, $1); add_brother($1, $3);}
    |   expression EQ expression                                {$$=create_node("Eq",0,0,Expression, $1); add_brother($1, $3);}
    |   expression NE expression                                {$$=create_node("Ne",0,0,Expression, $1); add_brother($1, $3);}
    |   expression PLUS expression                              {$$=create_node("Add",0,0,Expression, $1); add_brother($1, $3);}
    |   expression MINUS expression                             {$$=create_node("Sub",0,0,Expression, $1); add_brother($1, $3);}
    |   expression STAR expression                              {$$=create_node("Mul",0,0,Expression, $1); add_brother($1, $3);}
    |   expression DIV expression                               {$$=create_node("Div",0,0,Expression, $1); add_brother($1, $3);}
    |   expression MOD expression                               {$$=create_node("Mod",0,0,Expression, $1); add_brother($1, $3);}
    |   NOT expression  %prec  flag                             {$$=create_node("Not",0,0,Expression, $2);}
    |   MINUS expression %prec flag                             {$$=create_node("Minus",0,0,Expression, $2);}
    |   PLUS expression  %prec flag                             {$$=create_node("Plus",0,0,Expression, $2);}
    |   INTLIT                                                  {$$=create_node($1.val,$1.col,$1.l,intlit, NULL);}
    |   REALLIT                                                 {$$=create_node($1.val,$1.col,$1.l,reallit, NULL);}
    |   id_state                                                {$$=$1;}
    |   LPAR expression RPAR                                    {$$=$2;}
    |   function_invocation                                     {$$=$1;}
    |   LPAR error RPAR                                         {$$=(Structure*)NULL; is_error=1;}
    ;
%%

int lex_initiate(int argc, char* argv[]);
int return_line();
int return_column();

void print_annotated_tree(Structure *node, int number_of_points, char* scope, int inside_expression){
    Structure* tmp = node;
    char* another_scope = scope;
    int expression = inside_expression;
    int original_value = number_of_points;
    if(tmp != NULL){
        // Stuff for prints
        for(int i=0; i<number_of_points*2; i++){
            printf(".");
        }
        switch(tmp->type) {
            case id :
                if(inside_expression){
                    printf("Id(%s) - %s\n", tmp->token->val, type_to_string(search_variable(scope, tmp->token->val)->type));
                }else{
                    printf("Id(%s)\n", tmp->token->val);
                }
                break;
            case intlit :
                printf("IntLit(%s) - int\n", tmp->token->val);
                break;
            case reallit :
                printf("RealLit(%s) - float32\n", tmp->token->val);
                break;
            case strlit:
                printf("StrLit(%s) - string\n", tmp->token->val);
                break;
            case Expression:
                printf("%s\n - %s", tmp->token->val, type_to_string(tmp->value_type));
                expression = 1;
                break;
            case FuncDecl:
                printf("%s\n", tmp->token->val);
                another_scope = tmp->child->child->token->val;
                break;
            default :
                printf("%s\n", tmp->token->val);
        }
        number_of_points += 1;
        print_annotated_tree(node->child, number_of_points, another_scope, expression);
        print_annotated_tree(node->brother, original_value, scope, inside_expression);
        free(node->token);
        free(node);
    }
}

void print_tree(Structure *node, int number_of_points){
    Structure* tmp = node;
    int original_value = number_of_points;
    if(tmp != NULL){
        // Stuff for prints
        for(int i=0; i<number_of_points*2; i++){
            printf(".");
        }
        switch(tmp->type) {
            case id :
                printf("Id(%s)\n", tmp->token->val);
                break;
            case intlit :
                printf("IntLit(%s)\n", tmp->token->val);
                break;
            case reallit :
                printf("RealLit(%s)\n", tmp->token->val);
                break;
            case strlit:
                printf("StrLit(%s)\n", tmp->token->val);
                break;
            default :
                printf("%s\n", tmp->token->val);
        }
        number_of_points += 1;
        print_tree(node->child, number_of_points);
        print_tree(node->brother, original_value);
        free(node->token);
        free(node);
    }
}

void free_tree(Structure *node){
    if(node == NULL){
        return;
    }
    free_tree(node->child);
    free_tree(node->brother);
    free(node->token);
    free(node);
}

Structure* copy_type(Structure* type, Structure* brother){
    Structure* tmp = create_node(type->token->val, type->token->col, type->token->l, type->type, NULL);
    tmp->brother = brother;
    return tmp;
}

void yyerror (const char *s) { 
    printf("Line %d, column %d: syntax error: %s\n",return_line(),return_column(),yytext);
}

int main(int argc, char* argv[]){
    int is_semantic = 0;
    lex_initiate(argc,argv);
    if(argv[1] != NULL && strcmp(argv[1] , "-l")==0) {
        yylex();
        return 0;
    }
    yyparse();
    if(!is_error && argv[1] != NULL && strcmp(argv[1] , "-t")==0){
        print_tree(myprogram, 0);
    }else{
        free_tree(myprogram);
    }
    if(!is_error && argv[1] != NULL && strcmp(argv[1] , "-s")==0) {
        show_table();
        print_annotated_tree(myprogram, 0, "global", 0);
        return 0;
    }else{
        free_tree(myprogram);
    }
    return 0;
}
