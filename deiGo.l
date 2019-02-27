decimal_digit   [0-9]+
octal_digit     "0"[0-7]+
hex_digit       ("0x"|"0X")[0-9|a-f|A-F]+
exponent        ("e"|"E")("+"|"-")?{decimal_digit}
letter          [a-z|A-Z|_]+
%{
    int l = 1;
    int column = 1;
    int needs_semicolon = 0;

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

%X LINE_COMMENT  MULTI_LINE_COMMENT STRLIT
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
"package"       {printf("PACKAGE\n"); col(); semicollon(0);}
"return"        {printf("RETURN\n"); col(); semicollon(0);}
"else"          {printf("ELSE\n"); col(); semicollon(0);}
"if"            {printf("IF\n"); col(); semicollon(0);}
"for"           {printf("FOR\n"); col(); semicollon(0);}
"var"           {printf("VAR\n"); col(); semicollon(0);}
"fmt.Println"   {printf("PRINT\n"); col(); semicollon(0);}
"func"          {printf("FUNC\n"); col(); semicollon(0);} 
"strconv.Atoi"  {printf("PARSEINT\n"); col(); semicollon(0);}
"os.Args"       {printf("CMDARGS\n"); col(); semicollon(0);}

"int"           {printf("INT\n"); col(); semicollon(0);}
"float32"       {printf("FLOAT32\n"); col(); semicollon(0);}
"string"        {printf("STRING\n"); col(); semicollon(0);}
"bool"          {printf("BOOL\n"); col(); semicollon(0);}

{decimal_digit}|{octal_digit}|{hex_digit}                                       {printf("INTLIT(%s)\n",yytext); col(); semicollon(1);}
({decimal_digit}"."?{decimal_digit}?{exponent}?)|("."{decimal_digit}{exponent}?) {printf("REALIT(%s)\n",yytext); col(); semicollon(1);}
\"                         {BEGIN STRLIT; printf("STRLIT - ");}
<STRLIT>\"                {BEGIN 0; col(); semicollon(1);}
<STRLIT>[^\n\r\"""]+          {printf("%s\n",yytext); col();}
<STRLIT><<EOF>>                 {printf("line %d, column %d: unterminated string literal\n",l,column); BEGIN 0;}
<STRLIT>.                       {printf("line %d, column %d: invalid escape sequence (%s)\n",l,column,yytext); col(); semicollon(0);}


"++"|"--"|"break"|"case"|"chan"|"const"|"continue"|"default"|"defer"|"fallthrough"|"go"|"goto"|"import"|"interface"|"map"|"range"|"select"|"struct"|"switch"|"type" {printf("RESERVED\n"); col(); semicollon(0);}

 /*Operators* & punctuation*/
";"             {printf("SEMICOLON\n"); col();semicollon(0);}
"_"             {printf("BLANKID\n"); col(); semicollon(0);}
"+"             {printf("PLUS\n"); col(); semicollon(0);}
"-"             {printf("MINUS\n"); col(); semicollon(0);}
"*"             {printf("STAR\n"); col(); semicollon(0);}
"/"             {printf("DIV\n"); col(); semicollon(0);}
"%"             {printf("MOD\n"); col(); semicollon(0);}
"&&"            {printf("AND\n"); col(); semicollon(0);}
"||"            {printf("OR\n"); col(); semicollon(0);}
"<"             {printf("LT\n"); col(); semicollon(0);}
"<="            {printf("LE\n"); col(); semicollon(0);}
">"             {printf("GT\n"); col(); semicollon(0);}
">="            {printf("GE\n"); col(); semicollon(0);}
"="             {printf("ASSIGN\n"); col(); semicollon(0);}
"=="            {printf("EQ\n"); col(); semicollon(0);}
"!="            {printf("NE\n"); col(); semicollon(0);}
"!"             {printf("NOT\n"); col(); semicollon(0);}
","             {printf("COMMA\n"); col(); semicollon(0);}
"{"             {printf("LBRACE\n"); col(); semicollon(0);}
"}"             {printf("RBRACE\n"); col(); semicollon(1);}
"("             {printf("LPAR\n"); col(); semicollon(0);}
")"             {printf("RPAR\n"); col(); semicollon(1);}
"["             {printf("LSQ\n"); col(); semicollon(0);}
"]"             {printf("RSQ\n"); col(); semicollon(1);}



 /*Identifier*/
{letter}({letter}|{decimal_digit})* {printf("ID(%s)\n",yytext); col(); semicollon(1);}

 /*Special Cases*/
" "          {col();}
\n           {line(); if(needs_semicolon == 1){
					printf("SEMICOLON\n");
					needs_semicolon = 0;
			        }
                }
.            {printf("line %d, column %d: illegal character (%s)\n",l,column,yytext); col(); semicollon(0);}


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
