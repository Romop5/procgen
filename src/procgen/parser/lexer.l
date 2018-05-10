%x incl

%{
#include <cstdio>
#include <iostream>
#include <fstream>
#include "parser.hh"  
#include <procgen/parser/scanner.h>
#include <procgen/parser/generation.h>
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
%option nounistd

%%
%{
    LOC_STEP(generation._location);
%}

[\n]        { LOC_NEXT_LINE(generation._location,yyleng); LOC_STEP(generation._location);  
}
[ \t]        
"//"[^\n]* 


"/*"    {
        //ftp://ftp.gnu.org/old-gnu/Manuals/flex-2.5.4/html_mono/flex.html#SEC19
        int c;

        while((c = yyinput()) != 0)
            {
            if(c == '\n')
            {
                LOC_NEXT_LINE(generation._location,1); LOC_STEP(generation._location);  
            } else {
                LOC_STEP(generation._location);
            }

            if(c == '*')
                {
                if((c = yyinput()) == '/')
                    break;
                else
                    unput(c);
                }
            }
        }

"#include" BEGIN(incl);
<incl>[ \t]*      /* eat the whitespace */
<incl>\"[^\"]+\"  { /* got the include file name */
                std::string name = std::string(yytext).substr(1,yyleng-2);
                if(this->hasParserFileWithName(name) == false) 
                {
                   //std::cout << "Switching to file '" << yytext << "'" << std::endl; 
                   //std::cout << "Switching to file '" << name << "'" << std::endl;
                   // std::ifstream* includedFile = new std::ifstream(name,std::ifstream::in);
                    auto includedFile = std::make_shared<std::ifstream>(name, std::ifstream::in);
                    if(includedFile->is_open() == false)
                    {
                        //delete includedFile;
                        generation.errorMessage("Failed to load '%s' file in #include", name.c_str());
                        yyterminate();
                    }

                    this->includeFileStreams.push_back(includedFile);

                    this->addParsingFile(name.c_str());
                    this->includes.push(YY_CURRENT_BUFFER);
                    this->includeLocation.push(generation._location);
                
                    auto locationName = std::make_shared<std::string>(name);
                    this->identifiers.push_back(locationName);
                    generation._location = Parser::location_type(locationName.get(),0,0);

                    this->yy_switch_to_buffer(this->yy_create_buffer(includedFile.get(),YY_BUF_SIZE));
                }
                BEGIN(INITIAL);
            }
<<EOF>> {
            if(!this->includes.empty())
            {
                this->yy_delete_buffer(YY_CURRENT_BUFFER );
                this->yy_switch_to_buffer(this->includes.top());
                this->includes.pop();
                generation._location = this->includeLocation.top();
                this->includeLocation.pop();
            } else {
                yyterminate(); 
            }
        }

"parameter"	{ return token::PARAMETER;}
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
    auto identifierStringObject = std::make_shared<std::string>(yytext);
    this->identifiers.push_back(identifierStringObject);
	yylval->sval = identifierStringObject.get()->c_str();
	// detect if it is type or identifier
    if(generation.hasType(yytext))
		return token::TYPE;
	return token::NAME;
}

"+="		{ return token::ADDEQ;}
"-="		{ return token::MINEQ;}
"/="		{ return token::DIVEQ;}
"*="		{ return token::MULEQ;}


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
"!="		{ return token::NOTEQ;}
"&&"		{ return token::AND;}
"||"		{ return token::OR;}
"%"		{ return token::MODULO;}
"+"		{ return token::PLUS;}
"-"		{ return token::MINUS;}
"*"		{ return token::MUL;}
"/"		{ return token::DIV;}
">"		{ return token::GREATER;}
"<"		{ return token::LESS;}
"\."	{ return token::DOT;}

.      {
            generation.errorMessage("Invalid lexical token '%s'",yytext);
            yyterminate();
        }
%%


#ifdef yylex
#undef yylex
#endif

int ProcGenFlexLexer::yylex() {return 0;}
