%{
	#include <stdio.h>
	#include "../sys/sys_platform.h"
	
	#include "gsc.tab.hpp"
	#include "../symbols/symbol.h"
	
	#define YY_DECL int yylex (YYSTYPE *yylval_param, YYLTYPE *yylloc_param, void* yyscanner)
	#define YY_USER_ACTION yy_update_loc(yyscanner);
	
	YYLTYPE *yyget_lloc  (yyscan_t yyscanner);
	void yyset_lloc (YYLTYPE *  yylloc_param , void* yyscanner);
	char *yyget_text (void* yyscanner );
	
	void yy_update_loc(void* yyscanner)
	{
		YYLTYPE& yylloc = *yyget_lloc(yyscanner);
		char* text = yyget_text(yyscanner);
		
		yylloc.first_line = yylloc.last_line;
		yylloc.first_column = yylloc.last_column;
		
		for(int i = 0; text[i] != '\0'; i++)
		{
			if(text[i] == '\n')
			{
				yylloc.last_line++;
				yylloc.last_column = 0;
			}
			else
			{
				yylloc.last_column++;
			}
		}
	}
	
	#define PRINT_TOKEN(S) 
	//#define PRINT_TOKEN(S) printf(S)
%}

%option nodefault

%option reentrant
%option bison-bridge
%option bison-locations

%option nounput
%option nounistd
%option noyywrap

RX_T_STRING			\"(?:\\.|[^\"])*?\"|\'(?:\\.|[^\'])*?\'
RX_T_FLOAT			[0-9]+\.(?:[0-9]*)?f?|\.[0-9]+f?
RX_T_INT			[0-9]+
RX_T_FILEPATH		([_a-zA-Z0-9]+\\)+[_a-zA-Z0-9]+
RX_T_IDENTIFIER		[a-zA-Z_][a-zA-Z\-_0-9]*

RX_DEFAULT			(.|\n)

%x S_BLOCK_COMMENT
%x S_BLOCK_DEVSCRIPT

%%

[ \t\n\r]				/* skip whitespace & newlines */
"//".*					/* skip single line comments */
"/*"					{ BEGIN(S_BLOCK_COMMENT); }

<S_BLOCK_COMMENT>(.|\n)		/* ignore unicode shenanigans in comment blocks */
<S_BLOCK_COMMENT>"*/"		{ BEGIN(INITIAL); }

<INITIAL>"/#"				{ BEGIN(S_BLOCK_DEVSCRIPT); }
<S_BLOCK_DEVSCRIPT>"#/"		{ BEGIN(INITIAL); }	

{RX_T_STRING}		{ PRINT_TOKEN("T_STRING, "); yylval->literal_string = strdup(yytext); return T_STRING; }
{RX_T_FLOAT}		{ PRINT_TOKEN("T_FLOAT, "); yylval->literal_string = strdup(yytext); return T_FLOAT; }
{RX_T_INT}			{ PRINT_TOKEN("T_INT, "); yylval->literal_string = strdup(yytext); return T_INT; }

"#include"			{ PRINT_TOKEN("INCLUDE, "); return INCLUDE; }
"#using_animtree"	{ PRINT_TOKEN("USING_ANIMTREE, "); return USING_ANIMTREE; }
"#animtree"			{ PRINT_TOKEN("ANIMTREE, "); return ANIMTREE; }

\(			{ PRINT_TOKEN("OP_LPAREN, "); return OP_LPAREN; }
\)			{ PRINT_TOKEN("OP_RPAREN, "); return OP_RPAREN; }
\[\s*\[		{ PRINT_TOKEN("FUNC_POINTER_BEGIN, "); return FUNC_POINTER_BEGIN; }
\[			{ PRINT_TOKEN("OP_LBRACKET, "); return OP_LBRACKET; }	/*
\]\s*\]		{ PRINT_TOKEN("FUNC_POINTER_END, "); return FUNC_POINTER_END; } //Using this would override ']' in nested array expressions */
\]			{ PRINT_TOKEN("OP_RBRACKET, "); return OP_RBRACKET; }
\{			{ PRINT_TOKEN("OP_LBRACE, "); return OP_LBRACE; }
\}			{ PRINT_TOKEN("OP_RBRACE, "); return OP_RBRACE; }
\,			{ PRINT_TOKEN("OP_COMMA, "); return OP_COMMA; }
\.			{ PRINT_TOKEN("OP_DOT, "); return OP_DOT; }
\!\=		{ PRINT_TOKEN("OP_CMP_NEQ, "); return OP_CMP_NEQ; }
\!			{ PRINT_TOKEN("OP_NOT, "); return OP_NOT; }
%			{ PRINT_TOKEN("OP_MOD, "); return OP_MOD; }
\=\=		{ PRINT_TOKEN("OP_CMP_EQ, "); return OP_CMP_EQ; }
\=			{ PRINT_TOKEN("OP_ASSIGN, "); return OP_ASSIGN; }
\<\<		{ PRINT_TOKEN("OP_LSHIFT, "); return OP_LSHIFT; }
\<\=		{ PRINT_TOKEN("OP_CMP_LE, "); return OP_CMP_LE; }
\<			{ PRINT_TOKEN("OP_CMP_LT, "); return OP_CMP_LT; }
\>\>		{ PRINT_TOKEN("OP_RSHIFT, "); return OP_RSHIFT; }
\>\=		{ PRINT_TOKEN("OP_CMP_GE, "); return OP_CMP_GE; }
\>			{ PRINT_TOKEN("OP_CMP_GT, "); return OP_CMP_GT; }
\+\+		{ PRINT_TOKEN("OP_INC, "); return OP_INC; } //neither post nor pre
\+\=		{ PRINT_TOKEN("OP_ASSIGN_ADD, "); return OP_ASSIGN_ADD; }
\+			{ PRINT_TOKEN("OP_ADD, "); return OP_ADD; }
\-\-		{ PRINT_TOKEN("OP_DEC, "); return OP_DEC; } //neither post nor pre
\-\=		{ PRINT_TOKEN("OP_ASSIGN_SUB, "); return OP_ASSIGN_SUB; }
\-			{ PRINT_TOKEN("OP_SUB, "); return OP_SUB; }
\*\=		{ PRINT_TOKEN("OP_ASSIGN_MULT, "); return OP_ASSIGN_MULT; }
\*			{ PRINT_TOKEN("OP_MULT, "); return OP_MULT; }
\/=			{ PRINT_TOKEN("OP_ASSIGN_DIV, "); return OP_ASSIGN_DIV; }
\/			{ PRINT_TOKEN("OP_DIV, "); return OP_DIV; }
\|\|		{ PRINT_TOKEN("OP_CMP_OR, "); return OP_CMP_OR; }
\|\=		{ PRINT_TOKEN("OP_ASSIGN_BW_OR, "); return OP_ASSIGN_BW_OR; }
\|			{ PRINT_TOKEN("OP_BW_OR, "); return OP_BW_OR; }
\&\&		{ PRINT_TOKEN("OP_CMP_AND, "); return OP_CMP_AND; }
\&\=		{ PRINT_TOKEN("OP_ASSIGN_BW_AND, "); return OP_ASSIGN_BW_AND; }
\&			{ PRINT_TOKEN("OP_BW_AND, "); return OP_BW_AND; }
\^\=		{ PRINT_TOKEN("OP_ASSIGN_BW_XOR, "); return OP_ASSIGN_BW_XOR; }
\^			{ PRINT_TOKEN("OP_BW_XOR, "); return OP_BW_XOR; }
\~			{ PRINT_TOKEN("OP_BW_NOT, "); return OP_BW_NOT; }
\:\:		{ PRINT_TOKEN("OP_COLON_DOUBLE, "); return OP_COLON_DOUBLE; }
\:			{ PRINT_TOKEN("OP_COLON, "); return OP_COLON; }
\;			{ PRINT_TOKEN("OP_COLON_SEMI, "); return OP_COLON_SEMI; }
\?			{ PRINT_TOKEN("OP_QMARK, "); return OP_QMARK; } /*
// \@		{ PRINT_TOKEN("OP_AT, "); return OP_AT; } //disabled*/
\#			{ PRINT_TOKEN("OP_HASH, "); return OP_HASH; }

"if"			{ PRINT_TOKEN("T_IF, "); return T_IF; }
"else"			{ PRINT_TOKEN("T_ELSE, "); return T_ELSE; }
"switch"		{ PRINT_TOKEN("T_SWITCH, "); return T_SWITCH; }
"case"			{ PRINT_TOKEN("T_CASE, "); return T_CASE; }
"default"		{ PRINT_TOKEN("T_DEFAULT, "); return T_DEFAULT; }
"break"			{ PRINT_TOKEN("T_BREAK, "); return T_BREAK; }
"for"			{ PRINT_TOKEN("T_FOR, "); return T_FOR; }
"while"			{ PRINT_TOKEN("T_WHILE, "); return T_WHILE; }
"continue"		{ PRINT_TOKEN("T_CONTINUE, "); return T_CONTINUE; }
"return"		{ PRINT_TOKEN("T_RETURN, "); return T_RETURN; }

"thread"		{ PRINT_TOKEN("T_THREAD, "); return T_THREAD; }
"wait"			{ PRINT_TOKEN("T_WAIT, "); return T_WAIT; }

{RX_T_FILEPATH}		{ PRINT_TOKEN("T_FILEPATH, "); yylval->literal_string = strdup(yytext); return T_FILEPATH; }
{RX_T_IDENTIFIER}	{ PRINT_TOKEN("T_IDENTIFIER, "); yylval->t_identifier = strdup(yytext); return T_IDENTIFIER; }

<*>{RX_DEFAULT}		/* Ignore unexpected characters (this should never happen)
					{ fprintf(stderr, "RX_DEFAULT ACTION AT %d(%d) FOR '%s'\n", yyget_lloc(yyscanner)->first_line, yyget_lloc(yyscanner)->first_column, yytext);*/

%%
