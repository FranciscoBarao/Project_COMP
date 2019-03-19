decimal_digit   [0-9]+
octal_digit     "0"[0-7]+
hex_digit       ("0x"|"0X")[0-9a-fA-F]+
exponent        ("e"|"E")("+"|"-")?{decimal_digit}
letter          [a-zA-Z_]+
%{
    #include "y.tab.h"
    #define yygetc() getc(yyin)

    int l = 1;               //Var for Line
    int column = 1;          //Var for Column
    int needs_semicolon = 0; //Var for Semicolon need
    int tokens=1;            //Var for printing all tokens (-l -> 1)
    int error = 0;
    int column_error = 0;   //Var to save column in error case
    int line_error = 0;     //Var to save line in error case

    void col(){
        column += yyleng;
    }
    void line(){
        l++;
        column=1;
    }
    void semicollon(int x){
        needs_semicolon = (x==1 ? 1 : 0); 
    }
%}

 /*States*/
%X LINE_COMMENT  MULTI_LINE_COMMENT STRLIT SEQUENCE
%%

 /*Comments*/
"//"                    {col();BEGIN LINE_COMMENT;}
<LINE_COMMENT>"\n"      {BEGIN 0; line();}
<LINE_COMMENT>.         {col();}

"/*"                   {BEGIN MULTI_LINE_COMMENT; column_error = column; line_error = l; col();}
<MULTI_LINE_COMMENT><<EOF>>     {printf("Line %d, column %d: unterminated comment\n",line_error,column_error); BEGIN 0;}
<MULTI_LINE_COMMENT>"*/"        {BEGIN 0; col();}
<MULTI_LINE_COMMENT>\n          {line();}  
<MULTI_LINE_COMMENT>.           {col();}

 /*Keywords */
"package"       {if(tokens) printf("PACKAGE\n"); col(); semicollon(0); return PACKAGE;}
"return"        {if(tokens) printf("RETURN\n"); col(); semicollon(1); return RETURN;}
"else"          {if(tokens) printf("ELSE\n"); col(); semicollon(0); return ELSE;}
"if"            {if(tokens) printf("IF\n"); col(); semicollon(0); return IF;}
"for"           {if(tokens) printf("FOR\n"); col(); semicollon(0); return FOR;}
"var"           {if(tokens) printf("VAR\n"); col(); semicollon(0); return VAR;}
"fmt.Println"   {if(tokens) printf("PRINT\n"); col(); semicollon(0); return PRINT;}
"func"          {if(tokens) printf("FUNC\n"); col(); semicollon(0); return FUNC;} 
"strconv.Atoi"  {if(tokens) printf("PARSEINT\n"); col(); semicollon(0); return PARSEINT;}
"os.Args"       {if(tokens) printf("CMDARGS\n"); col(); semicollon(0); return CMDARGS;}

"int"           {if(tokens) printf("INT\n"); col(); semicollon(0); return INT;}
"float32"       {if(tokens) printf("FLOAT32\n"); col(); semicollon(0); return FLOAT32;}
"string"        {if(tokens) printf("STRING\n"); col(); semicollon(0); return STRING;}
"bool"          {if(tokens) printf("BOOL\n"); col(); semicollon(0); return BOOL;}

"++"|"--"|"break"|"case"|"chan"|"const"|"continue"|"default"|"defer"|"fallthrough"|"go"|"goto"|"import"|"interface"|"map"|"range"|"select"|"struct"|"switch"|"type" {if(tokens) printf("RESERVED(%s)\n",yytext); col(); semicollon(0); return RESERVED;}

{decimal_digit}|{octal_digit}|{hex_digit}                                         {if(tokens) printf("INTLIT(%s)\n",yytext); col(); semicollon(1); return INTLIT;}
({decimal_digit}"."?{decimal_digit}?{exponent}?)|("."{decimal_digit}{exponent}?)  {if(tokens) printf("REALLIT(%s)\n",yytext); col(); semicollon(1); return REALLIT;}

 /*Finish this*/
\"                                      {column_error = column; line_error=l; col(); BEGIN STRLIT;}
<STRLIT>\"                              {if(error==0 && yyleng>0){ yytext[yyleng-1]='\0'; printf("STRLIT(\"%s\")\n",yytext); semicollon(1);}col();error=0;BEGIN 0;}
<STRLIT>\\                              {yymore();col();BEGIN SEQUENCE;}
<STRLIT>.                               {yymore();col();}
<STRLIT>\n                              {printf("Line %d, column %d: unterminated string literal\n",line_error,column_error);semicollon(0);line();BEGIN 0;}
<STRLIT><<EOF>>                         {printf("Line %d, column %d: unterminated string literal\n",line_error,column_error);semicollon(0);BEGIN 0;}
<SEQUENCE>[^fnrt\"\\]                   {printf("Line %d, column %d: invalid escape sequence (\\%c)\n",line_error,column_error,yytext[yyleng-1]);error = 1; col(); semicollon(0);BEGIN STRLIT;}
<SEQUENCE>.                             {yymore();col();BEGIN STRLIT;}
<SEQUENCE><<EOF>>                       {printf("Line %d, column %d: unterminated string literal\n",line_error,column_error);semicollon(0);BEGIN 0;}

 /*Operators* & punctuation*/
";"             {if(tokens) printf("SEMICOLON\n"); col();semicollon(0); return SEMICOLON;}
"_"             {if(tokens) printf("BLANKID\n"); col(); semicollon(0); return BLANKID;}
"+"             {if(tokens) printf("PLUS\n"); col(); semicollon(0); return PLUS;}
"-"             {if(tokens) printf("MINUS\n"); col(); semicollon(0); return MINUS;}
"*"             {if(tokens) printf("STAR\n"); col(); semicollon(0); return STAR;}
"/"             {if(tokens) printf("DIV\n"); col(); semicollon(0); return DIV;}
"%"             {if(tokens) printf("MOD\n"); col(); semicollon(0); return MOD;}
"&&"            {if(tokens) printf("AND\n"); col(); semicollon(0); return AND;}
"||"            {if(tokens) printf("OR\n"); col(); semicollon(0); return OR;}
"<"             {if(tokens) printf("LT\n"); col(); semicollon(0); return LT;}
"<="            {if(tokens) printf("LE\n"); col(); semicollon(0); return LE;}
">"             {if(tokens) printf("GT\n"); col(); semicollon(0); return GT;}
">="            {if(tokens) printf("GE\n"); col(); semicollon(0); return GE;}
"="             {if(tokens) printf("ASSIGN\n"); col(); semicollon(0); return ASSIGN;}
"=="            {if(tokens) printf("EQ\n"); col(); semicollon(0); return EQ;}
"!="            {if(tokens) printf("NE\n"); col(); semicollon(0); return NE;}
"!"             {if(tokens) printf("NOT\n"); col(); semicollon(0); return NOT;}
","             {if(tokens) printf("COMMA\n"); col(); semicollon(0); return COMMA;}
"{"             {if(tokens) printf("LBRACE\n"); col(); semicollon(0); return LBRACE;}
"}"             {if(tokens) printf("RBRACE\n"); col(); semicollon(1); return RBRACE;}
"("             {if(tokens) printf("LPAR\n"); col(); semicollon(0); return LPAR;}
")"             {if(tokens) printf("RPAR\n"); col(); semicollon(1); return RPAR;}
"["             {if(tokens) printf("LSQ\n"); col(); semicollon(0); return LSQ;}
"]"             {if(tokens) printf("RSQ\n"); col(); semicollon(1); return RSQ;}

 /*Identifier*/
{letter}({letter}|{decimal_digit})* {if(tokens) printf("ID(%s)\n",yytext); col(); semicollon(1); return ID;}

 /*Special Cases*/
" "|"\t"          {col();}
\n           {line(); if(needs_semicolon == 1){
					if(tokens) printf("SEMICOLON\n");
					needs_semicolon = 0;
			        }
                }

<<EOF>>      { if(needs_semicolon == 1){
                if(tokens) printf("SEMICOLON\n"); 
                }
                return 0;
                }

.            {printf("Line %d, column %d: illegal character (%s)\n",l,column,yytext); col(); semicollon(0); BEGIN 0;}
%%
int main(int argc, char* argv[])
{
    if(argv[1] != NULL) {
        tokens = 1;
        }
    yylex();
    return 0;
}

int yywrap()
{
return 1;
}
