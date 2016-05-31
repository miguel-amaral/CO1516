%option c++ yyclass="zu_scanner" outfile="zu_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include "zu_scanner.h"
#include "zu_parser.tab.h"
// don't change this
#define yyerror LexerError

long value;
double auxDouble;
%}
%x X_STRING X_COMMENT X_STRING_IGNORE X_HEX_S
%%

"//".*$                 ; /* ignore comments */
"/*"                    yy_push_state(X_COMMENT);
<X_COMMENT>"*/"         yy_pop_state();
<X_COMMENT>"/*"         yy_push_state(X_COMMENT);
<X_COMMENT>.|\n         ; /* ignore comments */

">="                   	return tGE;
"<="                   	return tLE;
"=="                   	return tEQ;
"!="                   	return tNE;

"!!!"                  	return tRETURN;
"!!"                   	return tPRINTLN;
"><"			return tBREAK;
"<>"			return tCONTINUE;

[_A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tIDENTIFIER;

 /*Strings*/
\"                     yylval.s = new std::string(""); yy_push_state(X_STRING);
<X_STRING>\\\"         *yylval.s += yytext + 1;
<X_STRING>\\\\         *yylval.s += yytext + 1;
<X_STRING>\\n		   *yylval.s += '\n';
<X_STRING>\\r		   *yylval.s += '\r';
<X_STRING>\\t		   *yylval.s += '\t';
<X_STRING>\\[0-9a-fA-F]([0-9a-fA-F])?	{ int aux = strtol(yytext+1, nullptr, 16);
										  if(aux != 0)
	 										*yylval.s += (char) aux;
										  else
									  		yy_push_state(X_STRING_IGNORE);
										}
<X_STRING>\"           yy_pop_state(); return tSTRING;
<X_STRING>.            *yylval.s += yytext;

<X_STRING_IGNORE>\\\"	; /*Ignore this one*/
<X_STRING_IGNORE>\\\\	;
<X_STRING_IGNORE>\"	   	yyless(yyleng - 1);yy_pop_state();
<X_STRING_IGNORE>.|\n	; /*Ignore this one*/

 /* Integers */
[0][xX][0]*[1-9a-fA-F]+[0-9a-fA-F]*	{
										value = strtol(yytext+2, nullptr, 16);
										if(errno == 0){
											yylval.i = value;
											return tINTEGER;
										}
										else{ yyerror("Overflow Hex"); }
									}

(0|[1-9][0-9]*)					{
 										value = strtol(yytext, nullptr, 10);
 										if(errno == 0){
 											yylval.i = value;
 											return tINTEGER;
 										}
 										else{ yyerror("Overflow base 10 number"); }
 									}
 /*12.34e-24 Floats*/

([0-9]+"."[0-9]+|[0-9]+"."[0-9]+[Ee][0-9]+|[0-9]+[Ee][0-9]+|"."[0-9]+[Ee][0-9]+|"."[0-9]+) 		{
									   auxDouble = strtod(yytext, nullptr);
									   if(errno == 0){
                                                                                   yylval.d = auxDouble;
										   return tDOUBLE;
									   }
									   else{ yyerror("Overflow floats"); }
								   }


[-()<>=+*/%;@:,&|{}\[\]\.!?#\$]       return *yytext;

[ \t\r\n]+               ; /* ignore whitespace */

<*>.                      std::cout << "\e[31;1m[C] " << yytext << "[/C]\e[0m" ; yyerror("Unknown character");

%%
// Very, very dirty hack: flex is a mess generating C++ scanners.
int zu_scanner::yywrap() { return 1; }
