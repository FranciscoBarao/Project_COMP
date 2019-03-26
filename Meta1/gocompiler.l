decimal_digit   [0-9]+
octal_digit     "0"[0-7]+
hex_digit       ("0x"|"0X")[0-9a-fA-F]+
exponent        ("e"|"E")("+"|"-")?{decimal_digit}
letter          [a-zA-Z_]+

%{    
    int l = 1;               //Var for Line
    int column = 1;          //Var for Column
    int needs_semicolon = 0; //Var for Semicolon need
    int tokens=1;            //Var for printing all tokens (-l -> 1)
    int error = 0;
    int column_error = 0;   //Var to save column in error case
    int line_error = 0;     //Var to save line in error case
    int escape_col_error = 0;
    int escape_line_error = 0;

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
%X LINE_COMMENT  MULTI_LINE_COMMENT STRING SEQUENCE
%%

 /*Comments*/
"//"                    {col();BEGIN LINE_COMMENT;}
<LINE_COMMENT>"\n"      {BEGIN 0; line(); if(needs_semicolon == 1){
					if(tokens) printf("SEMICOLON\n");
					needs_semicolon = 0;
			        }
                }
<LINE_COMMENT>.         {col();}

"/*"                   {BEGIN MULTI_LINE_COMMENT; column_error = column; line_error = l; col();}
<MULTI_LINE_COMMENT><<EOF>>     {printf("Line %d, column %d: unterminated comment\n",line_error,column_error); semicollon(0); return 1;}
<MULTI_LINE_COMMENT>"*/"        {BEGIN 0; col();}
<MULTI_LINE_COMMENT>\n          {line();}  
<MULTI_LINE_COMMENT>.           {col();}

 /*Keywords */
"package"       {if(tokens) printf("PACKAGE\n"); col(); semicollon(0);}
"return"        {if(tokens) printf("RETURN\n"); col(); semicollon(1);}
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
({decimal_digit}"."?{decimal_digit}?{exponent}?)|("."{decimal_digit}{exponent}?)  {if(tokens) printf("REALLIT(%s)\n",yytext); col(); semicollon(1);}

 /*Finish this*/
\"                                      {if(error==1){error=0;} column_error = column; line_error=l; col(); BEGIN STRING;}
<STRING>\"                              {if(error==0 && yyleng>0){ yytext[yyleng-1]='\0'; printf("STRLIT(\"%s\")\n",yytext); semicollon(1);}col();error=0;BEGIN 0;}
<STRING>\\                              {yymore(); escape_line_error=l; escape_col_error=column+yyleng-1; BEGIN SEQUENCE;}
<STRING>.                               {yymore();}
<STRING>\n                              {printf("Line %d, column %d: unterminated string literal\n",line_error,column_error);semicollon(0);line();BEGIN 0;}
<STRING><<EOF>>                         {printf("Line %d, column %d: unterminated string literal\n",line_error,column_error);return 1;}
<SEQUENCE>[^fnrt\"\\]                   {printf("Line %d, column %d: invalid escape sequence (\\%c)\n",escape_line_error,escape_col_error,yytext[yyleng-1]);error = 1; col();BEGIN STRING;}
<SEQUENCE>.                             {yymore();BEGIN STRING;}
<SEQUENCE><<EOF>>                       {printf("Line %d, column %d: unterminated string literal\n",line_error,column_error);return 1;}

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

.            {printf("Line %d, column %d: illegal character (%s)\n",l,column,yytext); col(); BEGIN 0;}
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
