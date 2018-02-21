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


namespace ProcGen
{
    class Scanner: public ProcGenFlexLexer
    {
        public:
        Scanner(){}
        virtual ~Scanner(){}

        virtual Parser::token_type yylex(Parser::semantic_type* yylval,
                Parser::location_type* l, Generation& generation);
    };
    
}

#endif
