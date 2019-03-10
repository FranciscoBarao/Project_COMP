decimal_digit   [0-9]+
octal_digit     "0"[0-7]+
hex_digit       ("0x"|"0X")[0-9|a-f|A-F]+
exponent        ("e"|"E")("+"|"-")?{decimal_digit}
letter          [a-z|A-Z|_]+
%{
    #define yygetc() getc(yyin)

    int l = 1;               //Var for Line
    int column = 1;          //Var for Column
    int needs_semicolon = 0; //Var for Semicolon need
    int tokens=1;            //Var for printing all tokens (-l -> 1)
    int error = 0;

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
"//"                    {BEGIN LINE_COMMENT;}
<LINE_COMMENT>"\n"      {BEGIN 0; line();}
<LINE_COMMENT>.         {;}

"/*"                   { BEGIN MULTI_LINE_COMMENT; col();}
<MULTI_LINE_COMMENT><<EOF>>     {printf("Line %d, column %d: unterminated comment\n",l,column); BEGIN 0;}
<MULTI_LINE_COMMENT>"*/"        {BEGIN 0; col();}
<MULTI_LINE_COMMENT>\n          {line();}  
<MULTI_LINE_COMMENT>.           {col();}

 /*Keywords */
"package"       {if(tokens) printf("PACKAGE\n"); col(); semicollon(0);}
"return"        {if(tokens) printf("RETURN\n"); col(); semicollon(0);}
"else"          {if(tokens) printf("ELSE\n"); col(); semicollon(0);}
"if"            {if(tokens) printf("IF\n"); col(); semicollon(0);}
"for"           {if(tokens) printf("FOR\n"); col(); semicollon(0);}
"var"           {if(tokens) printf("VAR\n"); col(); semicollon(0);}
"fmt.Println"   {if(tokens) printf("PRINT\n"); col(); semicollon(0);}
"func"          {if(tokens) printf("FUNC\n"); col(); semicollon(0);} 
"strconv.Atoi"  {if(tokens) printf("PARSEINT\n"); col(); semicollon(0);}
"os.Args"       {if(tokens) printf("CMDARGS\n"); col(); semicollon(0);}

"int"           {if(tokens) printf("INT\n"); col(); semicollon(0);}
"float32"       {if(tokens) printf("FLOAT32\n"); col(); semicollon(0);}
"string"        {if(tokens) printf("STRING\n"); col(); semicollon(0);}
"bool"          {if(tokens) printf("BOOL\n"); col(); semicollon(0);}

"++"|"--"|"break"|"case"|"chan"|"const"|"continue"|"default"|"defer"|"fallthrough"|"go"|"goto"|"import"|"interface"|"map"|"range"|"select"|"struct"|"switch"|"type" {if(tokens) printf("RESERVED(%s)\n",yytext); col(); semicollon(0);}

{decimal_digit}|{octal_digit}|{hex_digit}                                         {if(tokens) printf("INTLIT(%s)\n",yytext); col(); semicollon(1);}
({decimal_digit}"."?{decimal_digit}?{exponent}?)|("."{decimal_digit}{exponent}?)  {if(tokens) printf("REALIT(%s)\n",yytext); col(); semicollon(1);}

 /*Finish this*/
\"                                      {col();BEGIN STRLIT;}
<STRLIT>\"                              {if(error==0 && yyleng>0){ yytext[yyleng-1]='\0'; printf("STRLIT(\"%s\")\n",yytext); semicollon(1);}col();error=0;BEGIN 0;}
<STRLIT>\\                              {yymore();col();BEGIN SEQUENCE;}
<STRLIT>.                               {yymore();col();}
<STRLIT>\n                              {printf("Line %d, column %d: unterminated string literal\n",l,column);semicollon(0);line();BEGIN 0;}
<STRLIT><<EOF>>                         {printf("Line %d, column %d: unterminated string literal\n",l,column);semicollon(0);BEGIN 0;}
<SEQUENCE>[^fnrt\"\\]                   {printf("Line %d, column %d: illegal character (\\%c)\n",l,column,yytext[yyleng-1]);error = 1; col(); semicollon(0);BEGIN STRLIT;}
<SEQUENCE>.                             {yymore();col();BEGIN STRLIT;}
<SEQUENCE><<EOF>>                       {printf("Line %d, column %d: unterminated string literal\n",l,column);semicollon(0);BEGIN 0;}

 /*Operators* & punctuation*/
";"             {if(tokens) printf("SEMICOLON\n"); col();semicollon(0);}
"_"             {if(tokens) printf("BLANKID\n"); col(); semicollon(0);}
"+"             {if(tokens) printf("PLUS\n"); col(); semicollon(0);}
"-"             {if(tokens) printf("MINUS\n"); col(); semicollon(0);}
"*"             {if(tokens) printf("STAR\n"); col(); semicollon(0);}
"/"             {if(tokens) printf("DIV\n"); col(); semicollon(0);}
"%"             {if(tokens) printf("MOD\n"); col(); semicollon(0);}
"&&"            {if(tokens) printf("AND\n"); col(); semicollon(0);}
"||"            {if(tokens) printf("OR\n"); col(); semicollon(0);}
"<"             {if(tokens) printf("LT\n"); col(); semicollon(0);}
"<="            {if(tokens) printf("LE\n"); col(); semicollon(0);}
">"             {if(tokens) printf("GT\n"); col(); semicollon(0);}
">="            {if(tokens) printf("GE\n"); col(); semicollon(0);}
"="             {if(tokens) printf("ASSIGN\n"); col(); semicollon(0);}
"=="            {if(tokens) printf("EQ\n"); col(); semicollon(0);}
"!="            {if(tokens) printf("NE\n"); col(); semicollon(0);}
"!"             {if(tokens) printf("NOT\n"); col(); semicollon(0);}
","             {if(tokens) printf("COMMA\n"); col(); semicollon(0);}
"{"             {if(tokens) printf("LBRACE\n"); col(); semicollon(0);}
"}"             {if(tokens) printf("RBRACE\n"); col(); semicollon(1);}
"("             {if(tokens) printf("LPAR\n"); col(); semicollon(0);}
")"             {if(tokens) printf("RPAR\n"); col(); semicollon(1);}
"["             {if(tokens) printf("LSQ\n"); col(); semicollon(0);}
"]"             {if(tokens) printf("RSQ\n"); col(); semicollon(1);}

 /*Identifier*/
{letter}({letter}|{decimal_digit})* {if(tokens) printf("ID(%s)\n",yytext); col(); semicollon(1);}

 /*Special Cases*/
" "          {col();}
\n|\t           {line(); if(needs_semicolon == 1){
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
