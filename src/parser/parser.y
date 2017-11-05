%{

#include <cstdio>
#include <iostream>

#include "interpret.h"
#include <memory> 
using namespace std;

// stuff from flex that bison needs to know about:
extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;


const std::string typeToString(size_t type);
void yyerror(Interpret& interpret, const char *s);
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
%token <ival> INT CHAR FLOAT BOOL 
%token <sval> STRING
%token KEYWORD_STRUCT
%token KEYWORD_INT
%token KEYWORD_FLOAT
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

%type<nodeId> type expr exprConst statements statement body assignment variableDefinition

%parse-param {Interpret& interpret} 
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
	type 
	{ std::cout << "it's a function" << std::endl; }
	STRING LPAR paramList RPAR body	

	{
		if($1 == INT) std::cout << "It's int" << std::endl;
		std::cout << $3 << "[p] Got function" << std::endl;

		interpret.vr->clear();
		auto returnResource = interpret.tr->sharedResource(typeToString($1));
		interpret.vr->addVar("return",returnResource);
		interpret.fr->addCompositeFunction($3, interpret.getNode($7),{}, returnResource);
		std::cout << "Registering: '"<< $3 << "' func" << std::endl;
	}

	;
paramList:
	| param
	| paramList COMMA param
	;
param:
     	type STRING
	;
body:
    	LBRAC
    	{
		auto body = std::make_shared<Body>();
		auto id = interpret.addNode(body);
		interpret.pushBody(id);
	}
	statements
	RBRAC
	{
		$$ = interpret.popBody();
		std::cout << "the end of body" << std::endl;
	}
statements:
	{}
	| statement
	{
		auto id = interpret.topBody();
		auto body = std::static_pointer_cast<Body>(interpret.getNode(id));
		body->stats.push_back(interpret.getNode($1));
	}
	| statements statement
	{
		auto id = interpret.topBody();
		auto body = std::static_pointer_cast<Body>(interpret.getNode(id));
		body->stats.push_back(interpret.getNode($1));
	}
;	
statement:
	variableDefinition
	| assignment

while:
     KEYWORD_WHILE LPAR expr RPAR LBRAC statements RBRAC
assignment:
	STRING EQ expr SEMICOL
	{
		$$ = interpret.createAssignment($1,$3);
	}

variableDefinition:
	type STRING SEMICOL
	{
		auto res = interpret.tr->sharedResource(typeToString($1));
		interpret.vr->addVar(std::string($2),res);
		$$ = interpret.createNOOP();
	}
	| type STRING EQ expr SEMICOL 
	{
		auto res = interpret.tr->sharedResource(typeToString($1));
		interpret.vr->addVar(std::string($2),res);
		$$ = interpret.createAssignment($2,$4);
	}
expr:
   	exprConst
	| expr MUL exprConst {interpret.createOperation("Mul", $1, $3);}
	| expr DIV exprConst {interpret.createOperation("Div", $1, $3);}
	| expr PLUS exprConst {interpret.createOperation("Plus", $1, $3);}
	| expr MINUS exprConst{interpret.createOperation("Minus", $1, $3);}
	| expr GREATER exprConst {interpret.createOperation("Greater", $1, $3);}
	| expr LESS exprConst {interpret.createOperation("Less", $1, $3);}
exprConst:
	 INT {interpret.createResource($1);}
	| FLOAT {interpret.createResource($1);}
	
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

	RunStatus stat;
	// get main()
	auto main = interpret.fr->getFunc("main");
	if(main != nullptr)
		(*main)(stat);
	else
		std::cerr << "main() missing" << std::endl;
	
}

void yyerror(Interpret& inter, const char *s) {
	cout << "Parse error!  Message: " << s << yylloc.first_line << "-" << yylloc.first_column <<  endl;
	// might as well halt now:
	exit(-1);
}

const std::string typeToString(size_t type)
{
	switch(type)
	{
		case KEYWORD_INT:
			return "int";
		case KEYWORD_FLOAT: 
			return "float";
		case KEYWORD_CHAR:
			return "char";
		default:
			return "unk";
	}
}


