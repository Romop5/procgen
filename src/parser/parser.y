%{

#include <cstdio>
#include <iostream>

#include "typereg.h"
#include "resource.h"
#include "functionreg.h"
#include <memory> 
using namespace std;

// stuff from flex that bison needs to know about:
extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;
 

class Interpret {
	private:
	std::map<size_t, std::shared_ptr<Statement>> nodes;
	std::map<size_t, std::shared_ptr<Resource>> resources;
	public:
	std::shared_ptr<FunctionReg> fr;
	std::shared_ptr<TypeRegister> tr;
	Interpret(std::shared_ptr<TypeRegister> tr, std::shared_ptr<FunctionReg> fr)
	{
		this->fr = fr;
		this->tr = tr;
	}
	size_t addNode(std::shared_ptr<Statement> st) 
	{
		static size_t i = 0;
		this->nodes[i] = st;
		return i++;
	}
	std::shared_ptr<Statement> getNode(size_t id) { return this->nodes[id];}

	size_t addResource(std::shared_ptr<Resource> st) 
	{
		static size_t i = 0;
		this->resources[i] = st;
		return i++;
	}
	std::shared_ptr<Resource> getResource(size_t id) { return this->resources[id];}
	
};


nodeId parser_addBinaryFunction(const std::string& name, nodeId resource, nodeId second

void yyerror(Interpret interpret, const char *s);
%}

%code requires {
	class Interpret;
}


// Bison fundamentally works by asking flex to get the next token, which it
// returns as an object of type "yystype".  But tokens could be of any
// arbitrary data type!  So we deal with that in Bison by defining a C union
// holding each of the types of tokens that Flex could return, and have Bison
// use that union instead of "int" for the definition of "yystype":
%union {
	int ival;
	float fval;
	char *sval;
	size_t nodeId;
}

// define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the union:
%token <ival> INT CHAR FLOAT BOOL STRING
%token KEYWORD_STRUCT
%token KEYWORD_INT
%token KEYWORD_CHAR
%token KEYWORD_BOOL
%token KEYWORD_SPLITER
%token KEYWORD_WHILE
%token LPAR
%token RPAR
%token LBRAC
%token RBRAC
%token SEMICOL
%token COMMA
%token EQ
// Operators
%token PLUS 
%token MINUS 
%token MUL 
%token DIV 
%token GREATER 
%token LESS  

%type<nodeId> type

%parse-param {Interpret interpret} 
%locations
%%
// this is the actual grammar that bison will parse, but for right now it's just
// something silly to echo to the screen what bison gets from flex.  We'll
// make a real one shortly:
program:
       	definitions {}
	;
definitions:
	definition {}
	| definitions definition {}
	;
definition:
	  structDefinition {}
	| functionDefinition {}
	;
structDefinition:
	  KEYWORD_STRUCT STRING LBRAC structMembers RBRAC SEMICOL{}
	;
structMembers:
		structMember {}
		| structMembers structMember {}
	;
structMember:
		type STRING SEMICOL {}
	;
functionDefinition:
		type STRING LPAR paramList RPAR LBRAC statements RBRAC 
		{
			if($1 == INT) std::cout << "It's int" << std::endl;
			std::cout << $2 << "[p] Got function" << std::endl;}

	;
paramList:
	| param
	| paramList COMMA param
	;
param:
     	type STRING
	;
statements:

	| statement
	| statements statement
	;	
statement:
	variableDefinition {std::cout << "[p] Got vardef" << std::endl;}
	| assignment
	| while {std::cout << "[p] Got while" << std::endl;}

while:
     KEYWORD_WHILE LPAR expr RPAR LBRAC statements RBRAC
assignment:
	STRING EQ expr SEMICOL
variableDefinition:
	type STRING SEMICOL
	| type STRING EQ expr SEMICOL 
expr:
   	exprConst
	| expr MUL exprConst {interpret.fr->getFunc(std::string("tMul:")+interpret->getResource(@1)->getTypeName())  
	| expr DIV exprConst
	| expr PLUS exprConst
	| expr MINUS exprConst
	| expr GREATER exprConst
	| expr LESS exprConst
exprConst:
	| INT {auto constVar = interpret.tr->sharedResource("int"); *(int*) constVar->value = $1; return interpret.addResource(constVar);}
	| FLOAT {auto constVar = interpret.tr->sharedResource("float"); *(float*) constVar->value = $1; return interpret.addResource(constVar);}
	
type:
    	KEYWORD_INT { $$ = INT;}
	| KEYWORD_CHAR { $$ = CHAR;}
	| KEYWORD_BOOL { $$ = BOOL;}
	;
%%

int main(int, char**) {
	auto tr = std::make_shared<TypeRegister>();
	auto fr = std::make_shared<FunctionReg>(tr);
	Interpret interpret(tr,fr);
	do {
		yyparse(interpret);
	} while (!feof(yyin));
	
}

void yyerror(Interpret inter, const char *s) {
	cout << "Parse error!  Message: " << s << yylloc.first_line << "-" << yylloc.first_column <<  endl;
	// might as well halt now:
	exit(-1);
}
