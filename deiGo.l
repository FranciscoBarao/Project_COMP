decimal_digit   [0-9]+
octal_digit     "0"[0-7]+
hex_digit       ("0x"|"0X")[0-9|a-f|A-F]+
exponent        ("e"|"E")("+"|"-")decimal_digit
letter          [a-z|A-Z|_]+
 /*"" -> entrar num state cause ".. -> error not closed " */
%{
    int line = 0;
    int column = 0;
%}

%X LINE_COMMENT  MULTI_LINE_COMMENT 
%%
 /*Comments*/
"//"                    {BEGIN LINE_COMMENT;}
<LINE_COMMENT>"\n"      {BEGIN 0;}
<LINE_COMMENT>.         {;}
"/*"                    {BEGIN GENERAL_COMMENT;}
<MULTI_LINE_COMMENT><<EOF>> {printf("Line %d, column %d")}
<MULTI_LINE_COMMENT>"*/"   {BEGIN 0;}
<MULTI_LINE_COMMENT>.      {;}



 /*Keywords */
"package"       {printf("PACKAGE");}
"return"        {printf("RETURN");}
"else"          {printf("ELSE");}
"if"            {printf("IF");}
"for"           {printf("FOR");}
"var"           {printf("VAR");}
"fmt.Println"   {printf("PRINT");}
"func"          {printf("FUNC");} /*Meter em FUNC state ??*/
"strconv.Atoi"  {printf("PARSEINT");}
"os.Args"       {printf("CMDARGS");}

"int"           {printf("INT");}
"float32"       {printf("FLOAT32");}
"string"        {printf("STRING");}
"bool"          {printf("BOOL");}

{decimal_digit}|{octal_digit}|{hex_digit}                                       {printf("INTLIT");}
({decimal_digit}("."{decimal_digit})?{exponent}?)|("."{decimal_digit}{exponent}?) {printf("REALIT");}
 /*STRLIT HERE*/

"++"|"--"|"break"|"case"|"chan"|"const"|"continue"|"default"|"defer"|"fallthrough"|"go"|"goto"|"import"|"interface"|"map"|"range"|"select"|"struct"|"switch"|"type" {printf("RESERVED");}

 /*Operators* & punctuation*/
";"             {printf("SEMICOLON");}
"_"             {printf("BLANKID");}
"+"             {printf("PLUS");}
"-"             {printf("MINUS");}
"*"             {printf("STAR");}
"/"             {printf("DIV");}
"%"             {printf("MOD");}
"&&"            {printf("AND");}
"||"            {printf("OR");}
"<"             {printf("LT");}
"<="            {printf("LE");}
">"             {printf("GT");}
">="            {printf("GE");}
"="             {printf("ASSIGN");}
"=="            {printf("EQ");}
"!="            {printf("NE");}
"!"             {printf("NOT");}
","             {printf("COMMA");}
"{"             {printf("LBRACE");}
"}"             {printf("RBRACE");}
"("             {printf("LPAR");}
")"             {printf("RPAR");}
"["             {printf("LSQ");}
"]"             {printf("RSQ");}



 /*Identifier*/
{letter}({letter}|{decimal_digit})* {printf("ID(%s)",yytext);}

.            {printf("%s",yytext);}
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
