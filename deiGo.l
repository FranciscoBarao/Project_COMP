decimal_digit   [0-9]+
octal_digit     "0"[0-7]+
hex_digit       ("0x"|"0X")[0-9|a-f|A-F]+
exponent        ("e"|"E")("+"|"-")?{decimal_digit}
letter          [a-z|A-Z|_]+
%{
    int l = 0;
    int column = 0;
    int needs_semicolon = 0;

    void col(){
        column += yyleng;
    }
    void line(){
        l++;
        column=0;
    }
%}

%X LINE_COMMENT  MULTI_LINE_COMMENT 
%%
 /*Comments*/
"//"                    {BEGIN LINE_COMMENT;}
<LINE_COMMENT>"\n"      {BEGIN 0; line();}
<LINE_COMMENT>.         {;}

"/*"                   { BEGIN MULTI_LINE_COMMENT; col();}
<MULTI_LINE_COMMENT><<EOF>> {printf("line %d, column %d: unterminated comment\n",l,column); BEGIN 0;}
<MULTI_LINE_COMMENT>"*/"   {BEGIN 0; col();}
<MULTI_LINE_COMMENT>\n      {line();}  
<MULTI_LINE_COMMENT>.      {col();}

 /*Keywords */
"package"       {printf("PACKAGE\n"); col();}
"return"        {printf("RETURN\n"); col();}
"else"          {printf("ELSE\n"); col();}
"if"            {printf("IF\n"); col();}
"for"           {printf("FOR\n"); col();}
"var"           {printf("VAR\n"); col();}
"fmt.Println"   {printf("PRINT\n"); col();}
"func"          {printf("FUNC\n"); col();} 
"strconv.Atoi"  {printf("PARSEINT\n"); col();}
"os.Args"       {printf("CMDARGS\n"); col();}

"int"           {printf("INT\n"); col();}
"float32"       {printf("FLOAT32\n"); col();}
"string"        {printf("STRING\n"); col();}
"bool"          {printf("BOOL\n"); col();}

{decimal_digit}|{octal_digit}|{hex_digit}                                       {printf("INTLIT(%s)\n",yytext); col();}
({decimal_digit}"."{decimal_digit}?{exponent}?)|("."{decimal_digit}{exponent}?) {printf("REALIT(%s)\n",yytext); col();}
 /*STRLIT HERE*/

"++"|"--"|"break"|"case"|"chan"|"const"|"continue"|"default"|"defer"|"fallthrough"|"go"|"goto"|"import"|"interface"|"map"|"range"|"select"|"struct"|"switch"|"type" {printf("RESERVED\n"); col();}

 /*Operators* & punctuation*/
";"             {printf("SEMICOLON\n"); col();needs_semicolon=0;}
"_"             {printf("BLANKID\n"); col();}
"+"             {printf("PLUS\n"); col();}
"-"             {printf("MINUS\n"); col();}
"*"             {printf("STAR\n"); col();}
"/"             {printf("DIV\n"); col();}
"%"             {printf("MOD\n"); col();}
"&&"            {printf("AND\n"); col();}
"||"            {printf("OR\n"); col();}
"<"             {printf("LT\n"); col();}
"<="            {printf("LE\n"); col();}
">"             {printf("GT\n"); col();}
">="            {printf("GE\n"); col();}
"="             {printf("ASSIGN\n"); col();}
"=="            {printf("EQ\n"); col();}
"!="            {printf("NE\n"); col();}
"!"             {printf("NOT\n"); col();}
","             {printf("COMMA\n"); col();}
"{"             {printf("LBRACE\n"); col();}
"}"             {printf("RBRACE\n"); col(); needs_semicolon=1;}
"("             {printf("LPAR\n"); col();}
")"             {printf("RPAR\n"); col(); needs_semicolon=1;}
"["             {printf("LSQ\n"); col();}
"]"             {printf("RSQ\n"); col(); needs_semicolon=1;}



 /*Identifier*/
{letter}({letter}|{decimal_digit})* {printf("ID(%s)\n",yytext); col(); needs_semicolon=1;}

 /*Special Cases*/
" "          {col();}
\n           {line(); if(needs_semicolon == 1){
					printf("SEMICOLON\n");
					needs_semicolon = 0;
			        }
                }
.            {printf("line %d, column %d: illegal character (%s)\n",l,column,yytext); col();}


%%
int main()
{
yylex();
return 0;
}

int yywrap()
{
return 1;
}
