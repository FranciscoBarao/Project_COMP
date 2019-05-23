decimal_digit   [0-9]+
octal_digit     "0"[0-7]+
hex_digit       ("0x"|"0X")[0-9a-fA-F]+
exponent        ("e"|"E")("+"|"-")?{decimal_digit}
letter          [a-zA-Z_]+
%{
    /*
        Pedro Ribeiro, 2016218753
        Francisco Barão, 2016238992
    */
    #include "functions.h"
    #include "y.tab.h"
    #define yygetc() getc(yyin)
   
    Scope_element *scope_table=NULL;

    int l = 1;               //Var for Line
    int column = 1;          //Var for Column
    int needs_semicolon = 0; //Var for Semicolon need
    int tokens=0;            //Var for printing all tokens (-l -> 1)
    int error = 0;
    int column_error = 0;   //Var to save column in error case
    int line_error = 0;     //Var to save line in error case
    int escape_col_error = 0;
    int escape_line_error = 0;
    int last_char = 0;
    int last_column = 0;
    int last_line = 0;

    void col(){
        last_char=0;
        column += yyleng;
    }
    void line(){
        last_line = l;
        last_column = column;
        last_char = 1;
        l++;
        column=1;
    }
    void col_eof(){
        column += yyleng;
    }
    void semicollon(int x){
        needs_semicolon = (x==1 ? 1 : 0); 
    }

    void identifier_to_token(){
        yylval.value.val = (char*)strdup(yytext); 
        yylval.value.l = l; 
        yylval.value.col = column-yyleng;
    }
%}

 /*States*/
%X LINE_COMMENT  MULTI_LINE_COMMENT STR_STATE SEQUENCE
%%

 /*Comments*/
"//"                    {BEGIN LINE_COMMENT;col();}
<LINE_COMMENT>"\n"      {BEGIN 0; line(); if(needs_semicolon == 1){
					if(tokens) printf("SEMICOLON\n");
					needs_semicolon = 0;
                    if(!tokens) return SEMICOLON;
			        }
                }
<LINE_COMMENT>.         {col();}

"/*"                   {BEGIN MULTI_LINE_COMMENT; column_error = column; line_error = l; col();}
<MULTI_LINE_COMMENT><<EOF>>     {printf("Line %d, column %d: unterminated comment\n",line_error,column_error); semicollon(0); return 1;}
<MULTI_LINE_COMMENT>"*/"        {BEGIN 0; col();}
<MULTI_LINE_COMMENT>\n          {line();}  
<MULTI_LINE_COMMENT>.           {col();}

 /*Keywords */
"package"       {if(tokens) printf("PACKAGE\n"); col(); semicollon(0); if(!tokens) return PACKAGE;}
"return"        {if(tokens) printf("RETURN\n"); col(); semicollon(1); if(!tokens){identifier_to_token(); return RETURN;}}
"else"          {if(tokens) printf("ELSE\n"); col(); semicollon(0); if(!tokens) return ELSE;}
"if"            {if(tokens) printf("IF\n"); col(); semicollon(0); if(!tokens) return IF;}
"for"           {if(tokens) printf("FOR\n"); col(); semicollon(0); if(!tokens) return FOR;}
"var"           {if(tokens) printf("VAR\n"); col(); semicollon(0); if(!tokens) return VAR;}
"fmt.Println"   {if(tokens) printf("PRINT\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return PRINT;}}
"func"          {if(tokens) printf("FUNC\n"); col(); semicollon(0); if(!tokens) return FUNC;} 
"strconv.Atoi"  {if(tokens) printf("PARSEINT\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return PARSEINT;}}
"os.Args"       {if(tokens) printf("CMDARGS\n"); col(); semicollon(0); if(!tokens) return CMDARGS;}

"int"           {if(tokens) printf("INT\n"); col(); semicollon(0); if(!tokens) return INT;}
"float32"       {if(tokens) printf("FLOAT32\n"); col(); semicollon(0); if(!tokens) return FLOAT32;}
"string"        {if(tokens) printf("STRING\n"); col(); semicollon(0); if(!tokens) return STRING;}
"bool"          {if(tokens) printf("BOOL\n"); col(); semicollon(0); if(!tokens) return BOOL;}

"++"|"--"|"break"|"case"|"chan"|"const"|"continue"|"default"|"defer"|"fallthrough"|"go"|"goto"|"import"|"interface"|"map"|"range"|"select"|"struct"|"switch"|"type" {if(tokens) printf("RESERVED(%s)\n",yytext); col(); semicollon(0); if(!tokens) return RESERVED;}

{decimal_digit}|{octal_digit}|{hex_digit}                                         {if(tokens) printf("INTLIT(%s)\n",yytext); col(); semicollon(1); if(!tokens){identifier_to_token(); return INTLIT;}}
({decimal_digit}"."?{decimal_digit}?{exponent}?)|("."{decimal_digit}{exponent}?)  {if(tokens) printf("REALLIT(%s)\n",yytext); col(); semicollon(1); if(!tokens){identifier_to_token(); return REALLIT;}}


\"                                      {BEGIN STR_STATE;if(error==1){error=0;}yymore(); column_error = column; line_error=l;}
<STR_STATE>\"                           {BEGIN 0; col(); if(error==0 && yyleng>0){if(tokens) printf("STRLIT(%s)\n",yytext); semicollon(1);} if(!tokens){identifier_to_token(); return STRLIT;} error=0;}
<STR_STATE>\\                           {BEGIN SEQUENCE;yymore(); escape_line_error=l; escape_col_error=column+yyleng-1;}
<STR_STATE>\n                           {BEGIN 0;printf("Line %d, column %d: unterminated string literal\n",line_error,column_error);semicollon(0);line();}
<STR_STATE><<EOF>>                      {printf("Line %d, column %d: unterminated string literal\n",line_error,column_error);return 1;}
<STR_STATE>.                            {yymore();}
<SEQUENCE>\n                            {BEGIN 0; printf("Line %d, column %d: unterminated string literal\n",line_error,column_error);semicollon(0);line();}
<SEQUENCE>[^fnrt\"\\]                   {BEGIN STR_STATE; printf("Line %d, column %d: invalid escape sequence (\\%c)\n",escape_line_error,escape_col_error,yytext[yyleng-1]); yymore(); error = 1;}
<SEQUENCE><<EOF>>                       {printf("Line %d, column %d: unterminated string literal\n",line_error,column_error);return 1;}
<SEQUENCE>.                             {BEGIN STR_STATE;yymore();}

 /*Operators* & punctuation*/
";"             {if(tokens) printf("SEMICOLON\n"); col();semicollon(0); if(!tokens) return SEMICOLON;}
"_"             {if(tokens) printf("BLANKID\n"); col(); semicollon(0); if(!tokens) return BLANKID;}
"+"             {if(tokens) printf("PLUS\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return PLUS;}}
"-"             {if(tokens) printf("MINUS\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return MINUS;}}
"*"             {if(tokens) printf("STAR\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return STAR;}}
"/"             {if(tokens) printf("DIV\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return DIV;}}
"%"             {if(tokens) printf("MOD\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return MOD;}}
"&&"            {if(tokens) printf("AND\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return AND;}}
"||"            {if(tokens) printf("OR\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return OR;}}
"<"             {if(tokens) printf("LT\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return LT;}}
"<="            {if(tokens) printf("LE\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return LE;}}
">"             {if(tokens) printf("GT\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return GT;}}
">="            {if(tokens) printf("GE\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return GE;}}
"="             {if(tokens) printf("ASSIGN\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return ASSIGN;}}
"=="            {if(tokens) printf("EQ\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return EQ;}}
"!="            {if(tokens) printf("NE\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return NE;}}
"!"             {if(tokens) printf("NOT\n"); col(); semicollon(0); if(!tokens){identifier_to_token(); return NOT;}}
","             {if(tokens) printf("COMMA\n"); col(); semicollon(0); if(!tokens) return COMMA;}
"{"             {if(tokens) printf("LBRACE\n"); col(); semicollon(0); if(!tokens) return LBRACE;}
"}"             {if(tokens) printf("RBRACE\n"); col(); semicollon(1); if(!tokens) return RBRACE;}
"("             {if(tokens) printf("LPAR\n"); col(); semicollon(0); if(!tokens) return LPAR;}
")"             {if(tokens) printf("RPAR\n"); col(); semicollon(1); if(!tokens) return RPAR;}
"["             {if(tokens) printf("LSQ\n"); col(); semicollon(0); if(!tokens) return LSQ;}
"]"             {if(tokens) printf("RSQ\n"); col(); semicollon(1); if(!tokens) return RSQ;}

 /*Identifier*/
{letter}({letter}|{decimal_digit})* {if(tokens) printf("ID(%s)\n",yytext); col(); semicollon(1); if(!tokens){identifier_to_token(); return ID;}}

 /*Special Cases*/
" "|"\t"          {col();}
\n           {line(); if(needs_semicolon == 1){
					if(tokens) printf("SEMICOLON\n");
					needs_semicolon = 0;
                    if(!tokens) return SEMICOLON;
			        }
                }

<<EOF>>      {if(needs_semicolon == 1){
                if(tokens) {
                    printf("SEMICOLON\n");
                }
                else{
                    needs_semicolon=0;
                    return SEMICOLON;
                    }
                }
                col_eof();
                return 0;
            }

.            {BEGIN 0; printf("Line %d, column %d: illegal character (%s)\n",l,column,yytext); col(); }
%%


int return_line(){
    if(last_char) return last_line;
    return l;
}
int return_column(){
    if(last_char){ return last_column;};
    return column-yyleng;
}


int lex_initiate(int argc, char* argv[]){
        if(argv[1] != NULL && strcmp(argv[1] , "-l")==0) {
            tokens = 1;
        }
    return 0;
}


int yywrap(){
return 1;
}
