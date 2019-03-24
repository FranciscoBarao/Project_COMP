/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INTLIT = 258,
    REALLIT = 259,
    Strlit = 260,
    ID = 261,
    INT = 262,
    FLOAT32 = 263,
    STRING = 264,
    BOOL = 265,
    OR = 266,
    AND = 267,
    LT = 268,
    GT = 269,
    EQ = 270,
    NE = 271,
    LE = 272,
    GE = 273,
    PLUS = 274,
    MINUS = 275,
    STAR = 276,
    DIV = 277,
    MOD = 278,
    NOT = 279,
    SEMICOLON = 280,
    BLANKID = 281,
    ASSIGN = 282,
    COMMA = 283,
    LBRACE = 284,
    RBRACE = 285,
    LPAR = 286,
    RPAR = 287,
    LSQ = 288,
    RSQ = 289,
    PACKAGE = 290,
    RETURN = 291,
    ELSE = 292,
    IF = 293,
    FOR = 294,
    VAR = 295,
    PRINT = 296,
    FUNC = 297,
    PARSEINT = 298,
    CMDARGS = 299,
    RESERVED = 300
  };
#endif
/* Tokens.  */
#define INTLIT 258
#define REALLIT 259
#define Strlit 260
#define ID 261
#define INT 262
#define FLOAT32 263
#define STRING 264
#define BOOL 265
#define OR 266
#define AND 267
#define LT 268
#define GT 269
#define EQ 270
#define NE 271
#define LE 272
#define GE 273
#define PLUS 274
#define MINUS 275
#define STAR 276
#define DIV 277
#define MOD 278
#define NOT 279
#define SEMICOLON 280
#define BLANKID 281
#define ASSIGN 282
#define COMMA 283
#define LBRACE 284
#define RBRACE 285
#define LPAR 286
#define RPAR 287
#define LSQ 288
#define RSQ 289
#define PACKAGE 290
#define RETURN 291
#define ELSE 292
#define IF 293
#define FOR 294
#define VAR 295
#define PRINT 296
#define FUNC 297
#define PARSEINT 298
#define CMDARGS 299
#define RESERVED 300

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
