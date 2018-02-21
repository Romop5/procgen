%{
#include <cstdio>
#include <iostream>
using namespace std;
//#define YY_DECL int yylex(ProcGen::Generation* proc)
//#define YY_DECL extern "C" int yylex()

#include "parser.hh"  
#include <procgen/parser/scanner.h>
#include "procgen.h" 
#include <memory>

#define yyterminate() return token::TOK_EOF

typedef ProcGen::Parser::token token;
typedef ProcGen::Parser::token_type token_type;

#define LOC_NEXT_LINE(name, count)\
    (name).lines(count); (name).step();

#define LOC_STEP(name)\
    (name).step();

#define LOC_MOVEBY(name,len)\
    (name) += len;

/*#define LOC_NEXT_LINE(name,count)\
    (name)->last_column = 1;                      \
    (name)->first_column= 1;                      \
    (name)->last_line += count;                  \
    (name)->first_line = (name).last_line;

#define LOC_STEP(name)\
    (name)->first_column = (name).last_column;     \
    (name)->first_line = (name).last_line;

#define LOC_MOVEBY(name,len)\
    (name)->last_column = (name).first_column; \
    (name)->first_column = (name).last_column+len;     \
*/
#define YY_USER_ACTION LOC_MOVEBY(generation._location,yyleng)
//#define YY_DECL int yylex(ProcGen::Generation* proc)


%}

%option noyywrap
%option yylineno
%option c++
%option prefix="ProcGen"
%option never-interactive

%%
%{
    LOC_STEP(generation._location);
%}

[\n]        { LOC_NEXT_LINE(generation._location,yyleng); LOC_STEP(generation._location);  
}
[ \t]        
"//"[^\n]* 
"/*"[^*]*"*/" 

"parameter"	{ return token::USING;}
"rule"		{ return token::RULE;}
"using"		{ return token::USING;}
"struct"	{ return token::STRUCT;}
"while"		{ return token::WHILE;}
"if"		{ return token::IF;}
"return"	{ return token::RETURN;}
"else"		{ return token::ELSE;}
"typeid"	{ return token::TYPEID;}
"convert"	{ return token::CONVERT;}
"at"		{ return token::AT;}
"size"		{ return token::SIZE;}
"del"		{ return token::DEL;}
"insert"	{ return token::INSERT;}
"="		    { return token::ASSIGN;}
"true"     { yylval->boolean = true; return token::BOOL;} 
"false"     { yylval->boolean = false; return token::BOOL;} 
[0-9]+\.[0-9]+ { yylval->fval = atof(yytext); return token::FLOAT; }

[0-9]+            { yylval->ival = atoi(yytext); return token::INTEGER; }
[a-zA-Z0-9]+      {
	//cout << "Got string: " << yytext << endl;
	yylval->sval = strdup(yytext);
	// detect if it is type or identifier
    if(generation.hasType(yytext))
		return token::TYPE;
	return token::NAME;
}
"!"		{ return token::NEGATION;}
","		{ return token::COMMA;}
"["		{ return token::LANGL;}
"]"		{ return token::RANGL;}
"{"		{ return token::LBRAC;}
"}"		{ return token::RBRAC;}
"("		{ return token::LPAR;}
")"		{ return token::RPAR;}
";"		{ return token::SEMICOL;}
"=="		{ return token::EQ;}
"+"		{ return token::PLUS;}
"-"		{ return token::MINUS;}
"*"		{ return token::MUL;}
"/"		{ return token::DIV;}
">"		{ return token::GREATER;}
"<"		{ return token::LESS;}
"\."	{ return token::DOT;}
.                ;

%%

#ifdef yylex
#undef yylex
#endif

int ProcGenFlexLexer::yylex() {return 0;}
