%{

#include <cstdio>
#include <iostream>

//#include "interpret.h"
#include <memory> 
#include <error.h>
using namespace std;

// stuff from flex that bison needs to know about:
extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;


const std::string typeToString(size_t type);
//void yyerror(Interpret& interpret, const char *s);
void yyerror(const char *s);
%}

/*%code requires {
	class Interpret;
}
*/

%union {
	int ival;
	float fval;
	char *sval;
	size_t nodeId;
}

%token <ival> INTEGER CHAR FLOAT BOOL
%token <sval> STRING

%token <sval> TYPE
%token <sval> name 

/* Keywords */
%token IF STRUCT USING RULE PARAMETER  ELSE WHILE
%token LPAR     "("
%token RPAR     ")"
%token LANGL    "["
%token RANGL    "]"
%token LBRAC    "{"
%token RBRAC    "}"
%token SEMICOL  ";"
%token COMMA    ","
%token EQ       "="
// Operators
%left EQ
%left GREATER LESS
%left MINUS 
%left PLUS 
%left DIV 
%left MUL 

/*%parse-param {Interpret& interpret}*/
%locations

%%
/* Program is a collection of declarations*/
program       : declarations
declarations  : declaration declarations | %empty

/*A declaration is either a using declaration, param declaration, or function declaration*/
declaration   : usingDeclaration | parameterDeclaration | functionDeclaration

/* Using declaration either declares a new type, type alias or defines a rules*/
usingDeclaration : USING name "=" usingVariant ";" 

usingVariant     : STRUCT "{" structureDeclaration "}"
                    |   type
                    |   RULE "{" statements "}" "{" statements "}" 


parameterDeclaration  : PARAMETER type name assign

assign                 : "=" literal ";" | ";" 


functionDeclaration   : type name "(" typeList ")" "{" statements "}" ";" 


structureDeclaration     : typeDeclaration ";" | structureDeclaration typeDeclaration ";" 


typeList                 : typeDeclaration | typeList "," typeDeclaration

typeDeclaration          : type name 


statements                : statement statements | %empty 

statement                 : functionCall ";" | declaration | assignment
                                | ifStatement | whileStatement 

declaration               : type name declarationEnd

declarationEnd           : ";" | "=" expression ";" 

functionCall             : name "(" argumentList ")" 

argumentList             : argument | argumentList "," argument

argument                  : expression

assignment                : name "=" expression


ifStatement              : IF "(" expression ")" "{" statements "}" elseClause
elseClause               : ELSE "{" statements "}" 

whileStatement           : WHILE "(" expression ")" "{" statements "}"

expression                : literal | functionCall 
                            |   expression "-" expression
                            |   expression "+" expression
                            |   expression "/" expression
                            |   expression "*" expression
                            |   "(" expression ")" 

/* Terminals*/

literal                   : INTEGER | STRING
type                      : TYPE

%%

int main(int argc, char** argv) {
/*
	if(argc < 2)
		error(1,0,"./parser <FILE>");
	yyin = fopen(argv[1],"r");
	if(yyin == NULL)
		error(1,0,"Failed to open file ...");

	auto tr = std::make_shared<TypeRegister>();
	auto fr = std::make_shared<FunctionReg>(tr);
	auto vr = std::make_shared<VariableReg>(tr);

	registerStandardTypes(tr.get());
	registerStandardFunctions(fr.get());

	Interpret interpret(tr,fr,vr);
	do {
		yyparse(interpret);
	} while (!feof(yyin));

	RunStatus stat;
	// get main()
	auto main = interpret.fr->getFunc("main");
	if(main != nullptr)
	{
		(*main)(stat);
		std::cout << "Done" << std::endl;
	}
	else
		std::cerr << "main() missing" << std::endl;
	
*/
}

//void yyerror(Interpret& inter, const char *s) {
void yyerror(const char *s) {
	cout << "Parse error!  Message: " << s << yylloc.first_line << "-" << yylloc.first_column <<  endl;
	// might as well halt now:
	exit(-1);
}

const std::string typeToString(size_t type)
{
	switch(type)
	{
		case INTEGER:
			return "int";
		case FLOAT: 
			return "float";
		case CHAR:
			return "char";
		default:
			return "unk";
	}
}


