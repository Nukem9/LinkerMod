/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "gsc.ypp" /* yacc.c:339  */

	#include <stdio.h>
	#include <stdlib.h>
	#include <iostream>
	
	#include "../symbols/symbol.h"
	#include "gsc.tab.hpp"
	#include "gsc.yy.h"

	int yylex(YYSTYPE *yylval_param, YYLTYPE *yylloc_param, void* yyscanner);
	extern void yyerror(YYLTYPE* loc, Symbol** yyAST_out, void* scanner, const char* err);

#line 79 "cpp/parser/gsc.tab.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "gsc.tab.hpp".  */
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
#line 30 "gsc.ypp" /* yacc.c:355  */

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

#line 137 "cpp/parser/gsc.tab.cpp" /* yacc.c:355  */

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
#line 58 "gsc.ypp" /* yacc.c:355  */

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

#line 251 "cpp/parser/gsc.tab.cpp" /* yacc.c:355  */
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

/* Copy the second part of user declarations.  */

#line 281 "cpp/parser/gsc.tab.cpp" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  15
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   493

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  37
/* YYNRULES -- Number of rules.  */
#define YYNRULES  117
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  234

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   328

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   211,   211,   217,   223,   231,   236,   239,   242,   243,
     244,   247,   252,   255,   261,   263,   265,   270,   272,   277,
     282,   287,   290,   294,   300,   306,   312,   318,   324,   333,
     338,   343,   345,   350,   356,   364,   369,   371,   373,   378,
     383,   391,   396,   397,   403,   404,   408,   410,   414,   416,
     418,   420,   422,   427,   429,   434,   438,   440,   442,   444,
     446,   448,   452,   454,   458,   460,   462,   464,   466,   468,
     470,   472,   474,   476,   478,   480,   482,   484,   486,   488,
     490,   492,   494,   496,   498,   500,   502,   504,   506,   508,
     510,   515,   519,   524,   526,   531,   538,   543,   545,   550,
     555,   558,   564,   566,   573,   574,   575,   576,   577,   578,
     579,   580,   581,   583,   585,   589,   601,   604
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "T_EOF", "error", "$undefined", "T_STRING", "T_FLOAT", "T_INT",
  "INCLUDE", "USING_ANIMTREE", "ANIMTREE", "OP_LPAREN", "OP_RPAREN",
  "FUNC_POINTER_BEGIN", "OP_LBRACE", "OP_RBRACE", "OP_COMMA", "OP_INC",
  "OP_DEC", "OP_COLON_DOUBLE", "OP_COLON", "OP_COLON_SEMI", "OP_QMARK",
  "OP_HASH", "T_IF", "T_ELSE", "T_SWITCH", "T_CASE", "T_DEFAULT",
  "T_BREAK", "T_FOR", "T_WHILE", "T_CONTINUE", "T_RETURN", "T_THREAD",
  "T_WAIT", "T_FILEPATH", "T_IDENTIFIER", "OP_LBRACKET", "OP_RBRACKET",
  "OP_DOT", "OP_NOT", "OP_BW_NOT", "OP_MULT", "OP_DIV", "OP_MOD", "OP_ADD",
  "OP_SUB", "OP_LSHIFT", "OP_RSHIFT", "OP_CMP_LT", "OP_CMP_LE",
  "OP_CMP_GT", "OP_CMP_GE", "OP_CMP_EQ", "OP_CMP_NEQ", "OP_BW_AND",
  "OP_BW_XOR", "OP_BW_OR", "OP_CMP_AND", "OP_CMP_OR", "OP_ASSIGN",
  "OP_ASSIGN_ADD", "OP_ASSIGN_SUB", "OP_ASSIGN_MULT", "OP_ASSIGN_DIV",
  "OP_ASSIGN_BW_AND", "OP_ASSIGN_BW_XOR", "OP_ASSIGN_BW_OR", "T_INVALID",
  "OP_INC_PRE", "OP_DEC_PRE", "OP_POSITIVE", "OP_NEGATIVE", "OP_INC_POST",
  "OP_DEC_POST", "$accept", "IncludeDirective", "AnimtreeDirective",
  "FunctionDeclaration", "FormalParameterList", "SourceElement",
  "SourceElements", "Program", "StringLiteral", "NumericLiteral", "Block",
  "FunctionParameterList", "FunctionCall", "FunctionExpression",
  "PointerExpression", "ReferenceExpression", "AnimReferenceExpression",
  "MemberExpression", "ElementList", "ListExpression", "ObjectExpression",
  "LiteralExpression", "OptionalExpression", "BasicExpression",
  "ModifiableExpression", "e", "Expression", "ExpressionStatement",
  "ReturnStatement", "WaitStatement", "EmptyStatement", "IfStatement",
  "SwitchStatement", "CaseStatement", "LoopStatement", "Statement",
  "StatementList", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328
};
# endif

#define YYPACT_NINF -198

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-198)))

#define YYTABLE_NINF -43

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      20,   -16,    14,    33,  -198,  -198,  -198,  -198,    22,    45,
      28,    46,    24,  -198,  -198,  -198,  -198,    54,  -198,    -6,
      57,    74,    42,  -198,  -198,  -198,  -198,   254,  -198,  -198,
    -198,  -198,  -198,   342,   124,  -198,   124,   124,    52,  -198,
      94,    83,   298,   114,    27,   106,   115,   117,   135,   123,
      35,   129,   342,   116,   136,   111,   342,   342,   126,   342,
     342,   147,  -198,  -198,  -198,  -198,  -198,   143,   153,  -198,
    -198,  -198,    84,  -198,  -198,    19,   356,   146,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,  -198,  -198,     2,   188,   152,
      16,    77,  -198,   153,    84,   119,   119,  -198,  -198,   342,
    -198,   342,   149,  -198,  -198,   342,   342,  -198,  -198,   159,
     162,   171,   172,   163,   148,   342,  -198,  -198,  -198,  -198,
    -198,  -198,  -198,   342,   342,   136,   342,   124,  -198,  -198,
    -198,   342,   342,   342,   342,   342,   342,   342,   342,   342,
     342,   342,   342,   342,   342,   342,   342,   342,   342,   342,
     342,   342,   342,   342,   342,   342,  -198,  -198,   342,  -198,
     342,   131,   150,   174,   176,  -198,   169,  -198,   179,  -198,
     342,   342,   342,  -198,  -198,    48,  -198,    51,    88,   155,
    -198,  -198,  -198,  -198,    65,    65,   132,   132,   323,   323,
     323,   323,   442,   442,   429,   415,   399,   382,   356,   356,
     356,   356,   356,   356,   356,   356,  -198,  -198,  -198,  -198,
     298,   298,   342,   298,    89,   107,   133,  -198,   342,  -198,
    -198,  -198,  -198,  -198,   175,  -198,  -198,  -198,  -198,  -198,
     342,   180,   298,  -198
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     8,     9,    10,    12,     0,     0,
       0,     0,     7,    13,    11,     1,     2,     0,     6,     0,
       0,     0,     0,     3,   117,     4,     5,     0,   116,    14,
      18,    17,    52,     0,     0,    19,     0,     0,     0,    96,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    53,     0,     0,     0,     0,     0,
       0,     0,    45,    44,   104,    30,    43,     0,    51,    89,
      54,    50,    48,    49,    55,    42,    91,     0,   106,   111,
     105,   114,   107,   108,   109,   110,   115,     0,    91,     0,
       0,     0,    42,     0,     0,    56,    57,    34,    16,     0,
      98,     0,     0,   101,   112,    47,     0,   113,    93,     0,
       0,     0,     0,     0,     0,    22,    38,    61,    60,    35,
      58,    59,    15,    22,    22,     0,     0,     0,    29,    62,
      63,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    92,    41,     0,    90,
       0,     0,     0,     0,     0,   100,     0,    46,     0,    94,
      22,    22,    22,    95,    33,     0,    21,     0,     0,     0,
      37,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    40,    39,    32,    31,
       0,     0,    47,     0,     0,     0,     0,    23,     0,    25,
      27,    36,    97,    99,     0,   102,    24,    26,    28,    20,
      47,     0,     0,   103
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -198,  -198,  -198,  -198,  -198,   185,  -198,  -198,  -198,  -198,
     178,   -87,   -70,  -198,   144,   -31,  -198,  -198,  -198,  -198,
     -27,   156,  -197,  -198,   -23,    72,   -33,  -198,  -198,  -198,
    -198,  -198,  -198,  -198,  -198,   -41,  -198
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,     6,    19,     7,     8,     9,    62,    63,
      64,   175,    65,    66,    67,    68,    69,    70,    87,    71,
      72,    73,   166,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    27
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      89,   100,   128,    90,    21,    93,    93,    91,    22,    94,
      94,    92,   157,    95,    96,   224,   158,   109,    10,   113,
     112,   128,    13,    11,   128,   124,     1,     2,     1,     2,
      29,    30,    31,   231,   129,   130,   177,   178,    29,    30,
      31,    93,    12,    32,    33,    15,    34,    16,    40,    17,
      36,    37,    38,   161,   108,     3,    40,     3,   217,    18,
      93,   219,   218,    93,    20,   218,   163,    51,   164,    53,
      54,    55,   167,   168,    56,    57,    23,    26,    58,    59,
      60,    61,   176,   214,   215,   216,    24,    97,    34,    61,
     176,   176,    99,   179,    38,    34,    93,    98,   220,   226,
     180,    38,   218,   218,    92,    88,   131,   132,   133,    51,
     128,    53,   125,   126,   162,   127,    51,   227,    53,   125,
     126,   218,   127,   101,   103,   206,   105,   207,   117,   118,
     -42,   120,   121,   114,   104,    34,   -42,   176,   176,   176,
      34,    38,   107,   228,   106,   115,    38,   218,   116,    93,
     122,   -42,   123,   -42,   -42,   -42,    51,   -42,    53,    54,
      55,   119,   124,    53,   110,   156,   160,   165,   208,   222,
     223,   170,   225,   131,   132,   133,   134,   135,   169,   167,
     171,   172,   173,   174,   210,   229,   211,   209,   212,   213,
     232,   233,   221,    14,   230,   111,     0,   167,   159,    25,
     102,     0,     0,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,     0,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,     0,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,    28,     0,    29,    30,    31,
       0,     0,    32,    33,     0,    34,    24,    35,     0,    36,
      37,    38,     0,    39,     0,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,     0,     0,    56,    57,     0,     0,    58,    59,    60,
       0,    29,    30,    31,     0,     0,    32,    33,    61,    34,
      24,     0,     0,    36,    37,    38,     0,    39,     0,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,     0,     0,    56,    57,     0,
       0,    58,    59,    60,     0,    29,    30,    31,     0,     0,
      32,    33,    61,    34,     0,     0,     0,    36,    37,    38,
       0,     0,     0,    40,   131,   132,   133,   134,   135,   136,
     137,     0,     0,     0,    51,     0,    53,    54,    55,     0,
       0,    56,    57,     0,     0,    58,    59,    60,     0,     0,
       0,     0,     0,     0,     0,     0,    61,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,     0,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,   145,   146,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,     0,   145,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141
};

static const yytype_int16 yycheck[] =
{
      33,    42,    72,    34,    10,    36,    37,    34,    14,    36,
      37,    34,    10,    36,    37,   212,    14,    50,    34,    52,
      51,    91,     0,     9,    94,     9,     6,     7,     6,     7,
       3,     4,     5,   230,    15,    16,   123,   124,     3,     4,
       5,    72,     9,     8,     9,     0,    11,    19,    21,     3,
      15,    16,    17,    37,    19,    35,    21,    35,    10,    35,
      91,    10,    14,    94,    10,    14,    99,    32,   101,    34,
      35,    36,   105,   106,    39,    40,    19,    35,    43,    44,
      45,    54,   115,   170,   171,   172,    12,    35,    11,    54,
     123,   124,     9,   126,    17,    11,   127,     3,    10,    10,
     127,    17,    14,    14,   127,    33,    41,    42,    43,    32,
     180,    34,    35,    36,    37,    38,    32,    10,    34,    35,
      36,    14,    38,     9,    18,   158,     9,   160,    56,    57,
      11,    59,    60,    17,    19,    11,    17,   170,   171,   172,
      11,    17,    19,    10,     9,     9,    17,    14,    37,   180,
       3,    32,     9,    34,    35,    36,    32,    38,    34,    35,
      36,    35,     9,    34,    35,    19,    14,    18,    37,   210,
     211,     9,   213,    41,    42,    43,    44,    45,    19,   212,
       9,     9,    19,    35,    10,   218,    10,    37,    19,    10,
      10,   232,    37,     8,    19,    51,    -1,   230,    10,    21,
      44,    -1,    -1,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,    -1,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    -1,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,     1,    -1,     3,     4,     5,
      -1,    -1,     8,     9,    -1,    11,    12,    13,    -1,    15,
      16,    17,    -1,    19,    -1,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    -1,    -1,    39,    40,    -1,    -1,    43,    44,    45,
      -1,     3,     4,     5,    -1,    -1,     8,     9,    54,    11,
      12,    -1,    -1,    15,    16,    17,    -1,    19,    -1,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    -1,    -1,    39,    40,    -1,
      -1,    43,    44,    45,    -1,     3,     4,     5,    -1,    -1,
       8,     9,    54,    11,    -1,    -1,    -1,    15,    16,    17,
      -1,    -1,    -1,    21,    41,    42,    43,    44,    45,    46,
      47,    -1,    -1,    -1,    32,    -1,    34,    35,    36,    -1,
      -1,    39,    40,    -1,    -1,    43,    44,    45,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    -1,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    -1,    56,    57,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    -1,    56,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     6,     7,    35,    75,    76,    77,    79,    80,    81,
      34,     9,     9,     0,    79,     0,    19,     3,    35,    78,
      10,    10,    14,    19,    12,    84,    35,   110,     1,     3,
       4,     5,     8,     9,    11,    13,    15,    16,    17,    19,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    39,    40,    43,    44,
      45,    54,    82,    83,    84,    86,    87,    88,    89,    90,
      91,    93,    94,    95,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    92,    99,   100,
      89,    94,    98,    89,    94,    98,    98,    35,     3,     9,
     109,     9,    95,    18,    19,     9,     9,    19,    19,   100,
      35,    88,    89,   100,    17,     9,    37,    99,    99,    35,
      99,    99,     3,     9,     9,    35,    36,    38,    86,    15,
      16,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    19,    10,    14,    10,
      14,    37,    37,   100,   100,    18,    96,   100,   100,    19,
       9,     9,     9,    19,    35,    85,   100,    85,    85,   100,
      94,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,   100,   100,    37,    37,
      10,    10,    19,    10,    85,    85,    85,    10,    14,    10,
      10,    37,   109,   109,    96,   109,    10,    10,    10,   100,
      19,    96,    10,   109
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    74,    75,    76,    77,    78,    78,    78,    79,    79,
      79,    80,    80,    81,    82,    82,    82,    83,    83,    84,
      85,    85,    85,    86,    86,    86,    86,    86,    86,    87,
      87,    88,    88,    89,    89,    90,    91,    91,    91,    92,
      92,    93,    94,    94,    95,    95,    96,    96,    97,    97,
      97,    97,    97,    98,    98,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,   100,   101,   102,   102,   103,   104,   105,   105,   106,
     107,   107,   108,   108,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   110,   110,   110
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     5,     5,     3,     1,     0,     1,     1,
       1,     2,     1,     2,     1,     2,     2,     1,     1,     3,
       3,     1,     0,     4,     5,     4,     5,     4,     5,     2,
       1,     4,     4,     3,     2,     2,     4,     3,     2,     3,
       3,     3,     1,     1,     1,     1,     1,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       3,     1,     2,     2,     3,     3,     1,     5,     2,     5,
       3,     2,     5,     9,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     1,     2,     2,     0
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, yyAST_out, scanner, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, yyAST_out, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, Symbol** yyAST_out, void* scanner)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  YYUSE (yyAST_out);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, Symbol** yyAST_out, void* scanner)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, yyAST_out, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, Symbol** yyAST_out, void* scanner)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , yyAST_out, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, yyAST_out, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, Symbol** yyAST_out, void* scanner)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (yyAST_out);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
          case 3: /* T_STRING  */
#line 204 "gsc.ypp" /* yacc.c:1257  */
      { free(((*yyvaluep).literal_string)); }
#line 1411 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 4: /* T_FLOAT  */
#line 204 "gsc.ypp" /* yacc.c:1257  */
      { free(((*yyvaluep).literal_string)); }
#line 1417 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 5: /* T_INT  */
#line 204 "gsc.ypp" /* yacc.c:1257  */
      { free(((*yyvaluep).literal_string)); }
#line 1423 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 34: /* T_FILEPATH  */
#line 204 "gsc.ypp" /* yacc.c:1257  */
      { free(((*yyvaluep).literal_string)); }
#line 1429 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 35: /* T_IDENTIFIER  */
#line 204 "gsc.ypp" /* yacc.c:1257  */
      { free(((*yyvaluep).t_identifier)); }
#line 1435 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 75: /* IncludeDirective  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1441 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 76: /* AnimtreeDirective  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1447 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 77: /* FunctionDeclaration  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1453 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 78: /* FormalParameterList  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1459 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 79: /* SourceElement  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1465 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 80: /* SourceElements  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1471 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 82: /* StringLiteral  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).literal); }
#line 1477 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 83: /* NumericLiteral  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).literal); }
#line 1483 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 84: /* Block  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1489 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 85: /* FunctionParameterList  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1495 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 86: /* FunctionCall  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).call); }
#line 1501 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 87: /* FunctionExpression  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1507 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 88: /* PointerExpression  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).pointer); }
#line 1513 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 89: /* ReferenceExpression  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).reference); }
#line 1519 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 90: /* AnimReferenceExpression  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1525 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 91: /* MemberExpression  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1531 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 92: /* ElementList  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1537 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 93: /* ListExpression  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1543 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 94: /* ObjectExpression  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1549 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 95: /* LiteralExpression  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).literal); }
#line 1555 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 96: /* OptionalExpression  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).expression); }
#line 1561 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 97: /* BasicExpression  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).expression); }
#line 1567 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 98: /* ModifiableExpression  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1573 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 99: /* e  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).expression); }
#line 1579 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 100: /* Expression  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).expression); }
#line 1585 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 101: /* ExpressionStatement  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1591 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 102: /* ReturnStatement  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1597 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 103: /* WaitStatement  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1603 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 104: /* EmptyStatement  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1609 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 105: /* IfStatement  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1615 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 106: /* SwitchStatement  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1621 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 107: /* CaseStatement  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1627 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 108: /* LoopStatement  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1633 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 109: /* Statement  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1639 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;

    case 110: /* StatementList  */
#line 203 "gsc.ypp" /* yacc.c:1257  */
      { delete ((*yyvaluep).symbol); }
#line 1645 "cpp/parser/gsc.tab.cpp" /* yacc.c:1257  */
        break;


      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (Symbol** yyAST_out, void* scanner)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

/* User initialization code.  */
#line 22 "gsc.ypp" /* yacc.c:1429  */
{
	#if VSCODE_COMPATIBLE_LOCATION
		yylloc.first_line = yylloc.last_line = 0;
		yylloc.first_column = yylloc.last_column = 0;
	#endif
}

#line 1761 "cpp/parser/gsc.tab.cpp" /* yacc.c:1429  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 212 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyval.symbol) = new Include(new Literal((yyvsp[-1].literal_string), (yylsp[-1]), S_TYPE_LITERAL_FILEPATH), (yyloc)); 
		}
#line 1952 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 3:
#line 218 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyval.symbol) = new Animtree(new Literal((yyvsp[-2].literal_string), (yylsp[-2]), S_TYPE_LITERAL_STRING), (yyloc)); 
		}
#line 1960 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 224 "gsc.ypp" /* yacc.c:1646  */
    {
		(yyval.symbol) = new Function(new Identifier((yyvsp[-4].t_identifier), (yylsp[-4])), (yyloc));
		(yyval.symbol)->AddChild(new Group((yyvsp[-2].symbol), (yylsp[-2])));
		(yyval.symbol)->AddChild((yyvsp[0].symbol));
	}
#line 1970 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 5:
#line 232 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyvsp[-2].symbol)->AddToEnd( new Identifier((yyvsp[0].t_identifier), (yylsp[0])) );
			(yyval.symbol) = (yyvsp[-2].symbol);
		}
#line 1979 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 237 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Identifier((yyvsp[0].t_identifier), (yylsp[0])); }
#line 1985 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 239 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = NULL; }
#line 1991 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 248 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyvsp[-1].symbol)->AddToEnd((yyvsp[0].symbol));
			(yyval.symbol) = (yyvsp[-1].symbol);
		}
#line 2000 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 256 "gsc.ypp" /* yacc.c:1646  */
    {
		*yyAST_out = new Group((yyvsp[-1].symbol), (yylsp[-1]));
	}
#line 2008 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 262 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.literal) = new Literal((yyvsp[0].literal_string), (yylsp[0]), S_TYPE_LITERAL_STRING); }
#line 2014 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 264 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.literal) = new Literal((yyvsp[0].literal_string), (yyloc), S_TYPE_LITERAL_STRING); }
#line 2020 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 266 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.literal) = new Literal((yyvsp[0].literal_string), (yyloc), S_TYPE_LITERAL_STRING); }
#line 2026 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 271 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.literal) = new Literal((yyvsp[0].literal_string), (yylsp[0]), S_TYPE_LITERAL_INT); }
#line 2032 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 273 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.literal) = new Literal((yyvsp[0].literal_string), (yylsp[0]), S_TYPE_LITERAL_FLOAT); }
#line 2038 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 278 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Group((yyvsp[-1].symbol), (yylsp[-1])); }
#line 2044 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 283 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyvsp[-2].symbol)->AddToEnd((yyvsp[0].expression));
			(yyval.symbol) = (yyvsp[-2].symbol);
		}
#line 2053 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 288 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = (yyvsp[0].expression); }
#line 2059 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 290 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = NULL; }
#line 2065 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 295 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyval.call) = new Call((yyloc), CALL_FLAG_IDENTIFIER);
			(yyval.call)->AddChild(new Identifier((yyvsp[-3].t_identifier),(yylsp[-3])));
			(yyval.call)->AddChild(new Group((yyvsp[-1].symbol), (yylsp[-1])));
		}
#line 2075 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 301 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyval.call) = new Call((yyloc), CALL_FLAG_THREAD | CALL_FLAG_IDENTIFIER);
			(yyval.call)->AddChild(new Identifier((yyvsp[-3].t_identifier),(yylsp[-3])));
			(yyval.call)->AddChild(new Group((yyvsp[-1].symbol), (yylsp[-1])));
		}
#line 2085 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 307 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyval.call) = new Call((yyloc), CALL_FLAG_POINTER);
			(yyval.call)->AddChild((yyvsp[-3].pointer));
			(yyval.call)->AddChild(new Group((yyvsp[-1].symbol), (yylsp[-1])));
		}
#line 2095 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 313 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyval.call) = new Call((yyloc), CALL_FLAG_THREAD | CALL_FLAG_POINTER);
			(yyval.call)->AddChild((yyvsp[-3].pointer));
			(yyval.call)->AddChild(new Group((yyvsp[-1].symbol), (yylsp[-1])));
		}
#line 2105 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 319 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyval.call) = new Call((yyloc), CALL_FLAG_REFERENCE);
			(yyval.call)->AddChild((yyvsp[-3].reference));
			(yyval.call)->AddChild(new Group((yyvsp[-1].symbol), (yylsp[-1])));
		}
#line 2115 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 325 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyval.call) = new Call((yyloc), CALL_FLAG_THREAD | CALL_FLAG_REFERENCE);
			(yyval.call)->AddChild((yyvsp[-3].reference));
			(yyval.call)->AddChild(new Group((yyvsp[-1].symbol), (yylsp[-1])));
		}
#line 2125 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 334 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyvsp[0].call)->SetCaller((yyvsp[-1].expression));
			(yyval.symbol) = (yyvsp[0].call);
		}
#line 2134 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 339 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = (yyvsp[0].call); }
#line 2140 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 344 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.pointer) = new Pointer((yyvsp[-2].expression), (yyloc)); }
#line 2146 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 346 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.pointer) = new Pointer((yyvsp[-2].expression), (yyloc)); }
#line 2152 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 351 "gsc.ypp" /* yacc.c:1646  */
    {
			Literal* file = new Literal((yyvsp[-2].literal_string), (yylsp[-2]), S_TYPE_LITERAL_FILEPATH);
			Identifier* identifier = new Identifier((yyvsp[0].t_identifier), (yylsp[0]));
			(yyval.reference) = new Reference(file, identifier, (yyloc));
		}
#line 2162 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 357 "gsc.ypp" /* yacc.c:1646  */
    {
			Identifier* identifier = new Identifier((yyvsp[0].t_identifier), (yylsp[0]));
			(yyval.reference) = new Reference(NULL, identifier, (yyloc));
		}
#line 2171 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 365 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Identifier((yyvsp[0].t_identifier), (yylsp[0])); }
#line 2177 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 370 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Member((yyvsp[-3].expression), (yyvsp[-1].expression), (yyloc), S_TYPE_MEMBER_ARRAY_ELEMENT); }
#line 2183 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 372 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Member((yyvsp[-2].expression), (yyvsp[0].expression), (yyloc), S_TYPE_MEMBER_OBJECT_PROPERTY); }
#line 2189 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 374 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Member(NULL, NULL, (yyloc), S_TYPE_MEMBER_ARRAY_EMPTY); }
#line 2195 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 379 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyvsp[-2].expression)->AddToEnd((yyvsp[0].expression));
			(yyval.symbol) = (yyvsp[-2].expression);
		}
#line 2204 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 384 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyvsp[-2].symbol)->AddToEnd((yyvsp[0].expression));
			(yyval.symbol) = (yyvsp[-2].symbol);
		}
#line 2213 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 392 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = (yyvsp[-1].symbol); }
#line 2219 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 410 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = NULL; }
#line 2225 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 415 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = (yyvsp[0].expression); }
#line 2231 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 417 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = (yyvsp[0].expression); }
#line 2237 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 419 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = (yyvsp[0].expression); }
#line 2243 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 421 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = (yyvsp[0].expression); }
#line 2249 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 423 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyloc)); }
#line 2255 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 428 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Identifier((yyvsp[0].t_identifier), (yylsp[0])); }
#line 2261 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 435 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = (Expression*)new Group((yyvsp[0].expression), (yylsp[0])); }
#line 2267 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 439 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression(OP_TYPE_INC, (yyvsp[0].expression), (yyloc)); }
#line 2273 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 441 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression(OP_TYPE_DEC, (yyvsp[0].expression), (yyloc)); }
#line 2279 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 443 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression(OP_TYPE_ADD, (yyvsp[0].expression), (yyloc)); }
#line 2285 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 59:
#line 445 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression(OP_TYPE_SUB, (yyvsp[0].expression), (yyloc)); }
#line 2291 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 447 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression(OP_TYPE_BW_NOT, (yyvsp[0].expression), (yyloc)); }
#line 2297 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 449 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression(OP_TYPE_NOT, (yyvsp[0].expression), (yyloc)); }
#line 2303 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 453 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-1].expression), OP_TYPE_INC, (yyloc)); }
#line 2309 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 455 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-1].expression), OP_TYPE_INC, (yyloc)); }
#line 2315 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 459 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_MULT, (yyvsp[0].expression), (yyloc)); }
#line 2321 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 461 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_DIV, (yyvsp[0].expression), (yyloc)); }
#line 2327 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 463 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_MOD, (yyvsp[0].expression), (yyloc)); }
#line 2333 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 465 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_ADD, (yyvsp[0].expression), (yyloc)); }
#line 2339 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 467 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_SUB, (yyvsp[0].expression), (yyloc)); }
#line 2345 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 469 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_LSHIFT, (yyvsp[0].expression), (yyloc)); }
#line 2351 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 471 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_RSHIFT, (yyvsp[0].expression), (yyloc)); }
#line 2357 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 473 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_CMP_LT, (yyvsp[0].expression), (yyloc)); }
#line 2363 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 475 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_CMP_LE, (yyvsp[0].expression), (yyloc)); }
#line 2369 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 477 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_CMP_GT, (yyvsp[0].expression), (yyloc)); }
#line 2375 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 479 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_CMP_GE, (yyvsp[0].expression), (yyloc)); }
#line 2381 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 481 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_CMP_EQ, (yyvsp[0].expression), (yyloc)); }
#line 2387 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 483 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_CMP_NEQ, (yyvsp[0].expression), (yyloc)); }
#line 2393 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 485 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_BW_AND, (yyvsp[0].expression), (yyloc)); }
#line 2399 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 487 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_BW_OR, (yyvsp[0].expression), (yyloc)); }
#line 2405 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 489 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_CMP_AND, (yyvsp[0].expression), (yyloc)); }
#line 2411 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 491 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_CMP_OR, (yyvsp[0].expression), (yyloc)); }
#line 2417 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 493 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_ASSIGN, (yyvsp[0].expression), (yyloc)); }
#line 2423 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 495 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_ASSIGN_ADD, (yyvsp[0].expression), (yyloc)); }
#line 2429 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 497 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_ASSIGN_SUB, (yyvsp[0].expression), (yyloc)); }
#line 2435 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 499 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_ASSIGN_MULT, (yyvsp[0].expression), (yyloc)); }
#line 2441 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 501 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_ASSIGN_DIV, (yyvsp[0].expression), (yyloc)); }
#line 2447 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 503 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_ASSIGN_BW_AND, (yyvsp[0].expression), (yyloc)); }
#line 2453 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 505 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_ASSIGN_BW_XOR, (yyvsp[0].expression), (yyloc)); }
#line 2459 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 507 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = new Expression((yyvsp[-2].expression), OP_TYPE_ASSIGN_BW_OR, (yyvsp[0].expression), (yyloc)); }
#line 2465 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 509 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = (yyvsp[0].expression); }
#line 2471 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 511 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.expression) = (yyvsp[-1].expression); }
#line 2477 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 520 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = (yyvsp[-1].expression); /*new Symbol(@$);*/ }
#line 2483 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 525 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Return(NULL, (yyloc)); }
#line 2489 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 527 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Return((yyvsp[-1].expression), (yyloc)); }
#line 2495 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 532 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Wait((yyvsp[-1].expression), (yyloc)); }
#line 2501 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 539 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Symbol((yyloc)); }
#line 2507 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 544 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Conditional((yyvsp[-2].expression), (yyvsp[0].symbol), (yyloc), S_TYPE_CONDITIONAL_IF); }
#line 2513 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 546 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Conditional(NULL, (yyvsp[0].symbol), (yyloc), S_TYPE_CONDITIONAL_ELSE); }
#line 2519 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 551 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Conditional((yyvsp[-2].expression), (yyvsp[0].symbol), (yyloc), S_TYPE_CONDITIONAL_SWITCH); }
#line 2525 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 556 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Conditional((yyvsp[-1].expression), NULL, (yyloc), S_TYPE_CONDITIONAL_CASE);
		}
#line 2532 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 101:
#line 559 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Conditional(NULL, NULL, (yyloc), S_TYPE_CONDITIONAL_CASE);
		}
#line 2539 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 102:
#line 565 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Conditional((yyvsp[-2].expression), (yyvsp[0].symbol), (yyloc), S_TYPE_CONDITIONAL_WHILE); }
#line 2545 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 103:
#line 567 "gsc.ypp" /* yacc.c:1646  */
    {
			(yyval.symbol) = new Conditional((yyvsp[-6].expression), (yyvsp[-4].expression), (yyvsp[-2].expression), (yyvsp[0].symbol), (yyloc), S_TYPE_CONDITIONAL_FOR);
		}
#line 2553 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 112:
#line 582 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Symbol((yyloc)); }
#line 2559 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 113:
#line 584 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = new Symbol((yyloc)); }
#line 2565 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 115:
#line 590 "gsc.ypp" /* yacc.c:1646  */
    {
			if((yyvsp[-1].symbol) != NULL)
			{
				(yyvsp[-1].symbol)->AddToEnd((yyvsp[0].symbol));
				(yyval.symbol) = (yyvsp[-1].symbol);
			}
			else
			{
				(yyval.symbol) = (yyvsp[0].symbol);
			}
		}
#line 2581 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;

  case 117:
#line 604 "gsc.ypp" /* yacc.c:1646  */
    { (yyval.symbol) = NULL; }
#line 2587 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
    break;


#line 2591 "cpp/parser/gsc.tab.cpp" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, yyAST_out, scanner, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, yyAST_out, scanner, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, yyAST_out, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp, yyAST_out, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, yyAST_out, scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, yyAST_out, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp, yyAST_out, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 607 "gsc.ypp" /* yacc.c:1906  */


//
// Notify the user that there was an error while parsing
// if error recovery is impossible, YYABORT is automatically called
// and the symbol tree is cleaned up
//
void yyerror(YYLTYPE* loc, Symbol** AST, yyscan_t scanner, const char* err) 
{
	fprintf(stderr, "PARSE ERROR AT LINE %d(%d): %s\n", loc->first_line, loc->first_column, err);
/*#if !(_DEBUG)
	exit(1);
#endif*/
}
