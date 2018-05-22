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

#ifndef YY_YY_CPP_PARSER_GSC_TAB_HPP_INCLUDED
# define YY_YY_CPP_PARSER_GSC_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 30 "gsc.ypp" /* yacc.c:1909  */

	#include "../sys/sys_platform.h"
	#include "../symbols/operator_enum.h"
	
	//
	// Forward declarations for any custom classes used in YYSTYPE
	//
	class Symbol;
	class Include;
	
	class Literal;
	class Identifier;
	
	class Conditional;
	
	class Animtree;
	class Expression;
	class Member;
	class Reference;
	class Pointer;
	
	class Function;
	class Call;
	
	class Return;
	class Wait;

#line 72 "cpp/parser/gsc.tab.hpp" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_EOF = 0,
    T_STRING = 258,
    T_FLOAT = 259,
    T_INT = 260,
    INCLUDE = 261,
    USING_ANIMTREE = 262,
    ANIMTREE = 263,
    OP_LPAREN = 264,
    OP_RPAREN = 265,
    FUNC_POINTER_BEGIN = 266,
    OP_LBRACE = 267,
    OP_RBRACE = 268,
    OP_COMMA = 269,
    OP_INC = 270,
    OP_DEC = 271,
    OP_COLON_DOUBLE = 272,
    OP_COLON = 273,
    OP_COLON_SEMI = 274,
    OP_QMARK = 275,
    OP_HASH = 276,
    T_IF = 277,
    T_ELSE = 278,
    T_SWITCH = 279,
    T_CASE = 280,
    T_DEFAULT = 281,
    T_BREAK = 282,
    T_FOR = 283,
    T_WHILE = 284,
    T_CONTINUE = 285,
    T_RETURN = 286,
    T_THREAD = 287,
    T_WAIT = 288,
    T_FILEPATH = 289,
    T_IDENTIFIER = 290,
    OP_LBRACKET = 291,
    OP_RBRACKET = 292,
    OP_DOT = 293,
    OP_NOT = 294,
    OP_BW_NOT = 295,
    OP_MULT = 296,
    OP_DIV = 297,
    OP_MOD = 298,
    OP_ADD = 299,
    OP_SUB = 300,
    OP_LSHIFT = 301,
    OP_RSHIFT = 302,
    OP_CMP_LT = 303,
    OP_CMP_LE = 304,
    OP_CMP_GT = 305,
    OP_CMP_GE = 306,
    OP_CMP_EQ = 307,
    OP_CMP_NEQ = 308,
    OP_BW_AND = 309,
    OP_BW_XOR = 310,
    OP_BW_OR = 311,
    OP_CMP_AND = 312,
    OP_CMP_OR = 313,
    OP_ASSIGN = 314,
    OP_ASSIGN_ADD = 315,
    OP_ASSIGN_SUB = 316,
    OP_ASSIGN_MULT = 317,
    OP_ASSIGN_DIV = 318,
    OP_ASSIGN_BW_AND = 319,
    OP_ASSIGN_BW_XOR = 320,
    OP_ASSIGN_BW_OR = 321,
    T_INVALID = 322,
    OP_INC_PRE = 323,
    OP_DEC_PRE = 324,
    OP_POSITIVE = 325,
    OP_NEGATIVE = 326,
    OP_INC_POST = 327,
    OP_DEC_POST = 328
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 58 "gsc.ypp" /* yacc.c:1909  */

	OPERATOR_TYPE	op; //operator
	
	int			literal_int;
	float		literal_float;
	char*		literal_string;
	
	char*		t_identifier;
	
	Symbol*		symbol;
	Include*	include;
	Animtree*	animtree;
	
	Literal*	literal;
	
	Member*		member;
	
	Expression*	expression;
	Reference*	reference;
	Pointer*	pointer;
	
	Function*	function;
	Call*		call;
	
	Return*		retn;

#line 186 "cpp/parser/gsc.tab.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int yyparse (Symbol** yyAST_out, void* scanner);

#endif /* !YY_YY_CPP_PARSER_GSC_TAB_HPP_INCLUDED  */
