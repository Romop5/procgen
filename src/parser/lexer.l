%{
#include <cstdio>
#include <iostream>
using namespace std;
#define YY_DECL extern "C" int yylex()

#include "parser.hh"  // to get the token types that we return
#include <memory>

%}

%option noyywrap
%option yylineno

%%
[ \t\n]           ;
"struct"	{cout << "Keyword: struct" << endl; return KEYWORD_STRUCT;}
"int"		{cout << "Keyword: int" << endl; return KEYWORD_INT;}
"bool"		{cout << "Keyword: bool" << endl; return KEYWORD_BOOL;}
"char"		{cout << "Keyword: char" << endl; return KEYWORD_CHAR;}
"while"		{cout << "Keyword: while" << endl; return KEYWORD_WHILE;}
"print"		{cout << "Keyword: print" << endl; return KEYWORD_PRINT;}
"return"	{cout << "Keyword: return" << endl; return KEYWORD_RETURN;}
"if"		{cout << "Keyword: if" << endl; return KEYWORD_IF;}
"="		{cout << "Keyword: =" << endl; return KEYWORD_ASSIGN;}
[0-9]+\.[0-9]+ { yylval.fval = atof(yytext); return FLOAT; }

[0-9]+            { yylval.ival = atoi(yytext); return INT; }
[a-zA-Z0-9]+      {
	// we have to copy because we can't rely on yytext not changing underneath us:
	cout << "Got string: " << yytext << endl;
	yylval.sval = strdup(yytext);
	return STRING;
}
":"		{cout << "Keyword: :" << endl; return KEYWORD_SPLITER;}
","		{cout << "Keyword: ," << endl; return COMMA;}
"{"		{cout << "Keyword: {" << endl; return LBRAC;}
"}"		{cout << "Keyword: }" << endl; return RBRAC;}
"("		{cout << "Keyword: (" << endl; return LPAR;}
")"		{cout << "Keyword: )" << endl; return RPAR;}
";"		{cout << "Keyword: ;" << endl; return SEMICOL;}
"=="		{cout << "Keyword: =" << endl; return EQ;}
"+"		{cout << "Keyword: +" << endl; return PLUS;}
"-"		{cout << "Keyword: -" << endl; return MINUS;}
"*"		{cout << "Keyword: *" << endl; return MUL;}
"/"		{cout << "Keyword: /" << endl; return DIV;}
">"		{cout << "Keyword: >" << endl; return GREATER;}
"<"		{cout << "Keyword: <" << endl; return LESS;}
.                ;

%%
