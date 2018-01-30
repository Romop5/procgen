%{
#include <cstdio>
#include <iostream>
using namespace std;
#define YY_DECL extern "C" int yylex()
#include "derivation.h"
#include "parser.hh"  // to get the token types that we return
#include <memory>

#define LOC_NEXT_LINE(name,count)\
    (name).last_column = 1;                      \
    (name).first_column= 1;                      \
    (name).last_line += count;                  \
    (name).first_line = (name).last_line;

#define LOC_STEP(name)\
    (name).first_column = (name).last_column;     \
    (name).first_line = (name).last_line;

#define LOC_MOVEBY(name,len)\
    (name).last_column = (name).first_column; \
    (name).first_column = (name).last_column+len;     \

#define YY_USER_ACTION LOC_MOVEBY(yylloc,yyleng)

%}

%option noyywrap
%option yylineno

%%
%{
    printf("Nigga\n");
    LOC_STEP(yylloc);
%}

[\n+]        { LOC_NEXT_LINE(yylloc,yyleng); LOC_STEP(yylloc);  }
[ \t]        

"parameter"	{cout << "Keyword: param" << endl; return USING;}
"using"	{cout << "Keyword: using" << endl; return USING;}
"struct"	{cout << "Keyword: struct" << endl; return STRUCT;}
"while"		{cout << "Keyword: while" << endl; return WHILE;}
"if"		{cout << "Keyword: if" << endl; return IF;}
"="		{cout << "Keyword: =" << endl; return EQ;}
[0-9]+\.[0-9]+ { yylval.fval = atof(yytext); return FLOAT; }

[0-9]+            { yylval.ival = atoi(yytext); return INTEGER; }
[a-zA-Z0-9]+      {
	// we have to copy because we can't rely on yytext not changing underneath us:
	cout << "Got string: " << yytext << endl;
	yylval.sval = strdup(yytext);
    // detect if it is type or identifier
    
    if(strcmp(yylval.sval,"int") == 0)
        return TYPE;
	return NAME;
}
","		{cout << "Keyword: ," << endl; return COMMA;}
"["		{cout << "Keyword: [" << endl; return LANGL;}
"]"		{cout << "Keyword: ]" << endl; return RANGL;}
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
