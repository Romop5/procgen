/**
 * @file 'parser/scanner.h'
 * @brief Class for wrapping Flex into C++ 
 * @copyright The MIT license 
 */
#ifndef SCANPIT_HH_
#define SCANPIT_HH_

#include "parser.hh"
#include <memory>

#ifndef YY_DECL
#define YY_DECL ProcGen::Parser::token_type                     \
ProcGen::Scanner::yylex(ProcGen::Parser::semantic_type* yylval, \
    ProcGen::Parser::location_type* loc,                        \
    ProcGen::Generation& generation)
#endif

#ifndef __FLEX_LEXER_H
#define yyFlexLexer ProcGenFlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#endif

#include <fstream>
#include <set>
#include <stack>

namespace ProcGen {
class Scanner : public ProcGenFlexLexer {
public:
    Scanner()
    {
        yy_flex_debug = 0;
    }

    virtual ~Scanner() {}

    virtual Parser::token_type yylex(Parser::semantic_type* yylval,
        Parser::location_type* l, Generation& generation);

    //std::stack<ProcGen_BUFFER_STATE> includes;
    std::stack<yy_buffer_state*> includes;
    std::stack<Parser::location_type> includeLocation;

    std::set<std::string> alreadyParsed;

    std::vector<std::shared_ptr<std::string>> identifiers;
    std::vector<std::shared_ptr<std::ifstream>> includeFileStreams;

    bool hasParserFileWithName(const std::string& name)
    {
        if (alreadyParsed.find(name) != alreadyParsed.end())
            return true;
        return false;
    }
    void addParsingFile(const std::string& name) { alreadyParsed.insert(name); }
};
}

#endif
