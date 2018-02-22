#ifndef SCANPIT_HH_
# define SCANPIT_HH_

# include "parser.hh"


# ifndef YY_DECL
#  define YY_DECL ProcGen::Parser::token_type                         \
     ProcGen::Scanner::yylex(ProcGen::Parser::semantic_type* yylval,    \
                              ProcGen::Parser::location_type* loc,        \
                              ProcGen::Generation& generation)
# endif


# ifndef __FLEX_LEXER_H
#  define yyFlexLexer ProcGenFlexLexer
#  include <FlexLexer.h>
#  undef yyFlexLexer
# endif

#include <stack>
#include <fstream>
#include <set>

namespace ProcGen
{
    class Scanner: public ProcGenFlexLexer
    {
        public:
        Scanner(){
            yy_flex_debug = 0;
        }
        
        virtual ~Scanner(){}

        virtual Parser::token_type yylex(Parser::semantic_type* yylval,
                Parser::location_type* l, Generation& generation);

        //std::stack<ProcGen_BUFFER_STATE> includes;
        std::stack<yy_buffer_state*> includes;
        std::stack<Parser::location_type> includeLocation;

        std::set<std::string> alreadyParsed;

        
        bool hasParserFileWithName(const std::string& name)  
        {
            if(alreadyParsed.find(name) != alreadyParsed.end())
                return true;
            return false;
        }
        void addParsingFile(const std::string& name) { alreadyParsed.insert(name); }
    };
    
}

#endif
