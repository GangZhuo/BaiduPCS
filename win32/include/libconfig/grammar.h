
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOK_BOOLEAN = 258,
     TOK_INTEGER = 259,
     TOK_HEX = 260,
     TOK_INTEGER64 = 261,
     TOK_HEX64 = 262,
     TOK_FLOAT = 263,
     TOK_STRING = 264,
     TOK_NAME = 265,
     TOK_EQUALS = 266,
     TOK_NEWLINE = 267,
     TOK_ARRAY_START = 268,
     TOK_ARRAY_END = 269,
     TOK_LIST_START = 270,
     TOK_LIST_END = 271,
     TOK_COMMA = 272,
     TOK_GROUP_START = 273,
     TOK_GROUP_END = 274,
     TOK_SEMICOLON = 275,
     TOK_GARBAGE = 276,
     TOK_ERROR = 277
   };
#endif
/* Tokens.  */
#define TOK_BOOLEAN 258
#define TOK_INTEGER 259
#define TOK_HEX 260
#define TOK_INTEGER64 261
#define TOK_HEX64 262
#define TOK_FLOAT 263
#define TOK_STRING 264
#define TOK_NAME 265
#define TOK_EQUALS 266
#define TOK_NEWLINE 267
#define TOK_ARRAY_START 268
#define TOK_ARRAY_END 269
#define TOK_LIST_START 270
#define TOK_LIST_END 271
#define TOK_COMMA 272
#define TOK_GROUP_START 273
#define TOK_GROUP_END 274
#define TOK_SEMICOLON 275
#define TOK_GARBAGE 276
#define TOK_ERROR 277




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 85 "grammar.y"

  int ival;
  long long llval;
  double fval;
  char *sval;



/* Line 1676 of yacc.c  */
#line 105 "grammar.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




