%{
#include <cstdio>
#include <iostream>
using namespace std;
#define YY_DECL int yylex(ProcGen::Generation* proc)
//#define YY_DECL extern "C" int yylex()
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
//#define YY_DECL int yylex(ProcGen::Generation* proc)

%}

%option noyywrap
%option yylineno

%%
%{
    LOC_STEP(yylloc);
%}

[\n]        { LOC_NEXT_LINE(yylloc,yyleng); LOC_STEP(yylloc);  }
[ \t]        
"//"[^\n]* 
"/*"[^*]*"*/" 

"parameter"	{ return USING;}
"rule"		{ return RULE;}
"using"		{ return USING;}
"struct"	{ return STRUCT;}
"while"		{ return WHILE;}
"if"		{ return IF;}
"return"	{ return RETURN;}
"else"		{ return ELSE;}
"="		    { return ASSIGN;}
"true"      { yylval.boolean = true; return BOOL;} 
"false"     { yylval.boolean = false; return BOOL;} 
[0-9]+\.[0-9]+ { yylval.fval = atof(yytext); return FLOAT; }

[0-9]+            { yylval.ival = atoi(yytext); return INTEGER; }
[a-zA-Z0-9]+      {
	cout << "Got string: " << yytext << endl;
	yylval.sval = strdup(yytext);
	// detect if it is type or identifier
    if(proc->hasType(yytext))
		return TYPE;
	return NAME;
}
","		{ return COMMA;}
"["		{ return LANGL;}
"]"		{ return RANGL;}
"{"		{ return LBRAC;}
"}"		{ return RBRAC;}
"("		{ return LPAR;}
")"		{ return RPAR;}
";"		{ return SEMICOL;}
"=="		{ return EQ;}
"+"		{ return PLUS;}
"-"		{ return MINUS;}
"*"		{ return MUL;}
"/"		{ return DIV;}
">"		{ return GREATER;}
"<"		{ return LESS;}
"\."	{ return DOT;}
.                ;

%%
