%{

#include <cstdio>
#include <iostream>

#include "interpret.h"
#include <memory> 
#include <error.h>
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
%token KEYWORD_PRINT
%token KEYWORD_RETURN
%token KEYWORD_IF
%token KEYWORD_ASSIGN
%token LPAR
%token RPAR
%token LANGL
%token RANGL
%token LBRAC
%token RBRAC
%token SEMICOL
%token COMMA
%token EQ
// Operators
%left EQ
%left GREATER LESS
%left MINUS 
%left PLUS 
%left DIV 
%left MUL 

%type<nodeId> type expr exprConst statements statement body assignment variableDefinition printStatement functionCall returnStatement whileStatement 
		ifStatement

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
	KEYWORD_STRUCT STRING LBRAC structMembers RBRAC SEMICOL 
	{
		// define a new type	
		interpret.tr->addComposite($2,interpret.structDef);
		interpret.structDef.clear();
	}
	;
structMembers:
	structMember {}
	| structMembers structMember {}
	;
structMember:
	type STRING SEMICOL {
		interpret.structDef.push_back($1);
	}
	;
functionDefinition:
	type 
	{ 
		std::cout << "it's a function" << std::endl; 
		interpret.paramClear();
		interpret.vr->clear();
		auto returnResource = interpret.tr->sharedResource($1);
		interpret.vr->addVar("return",returnResource);
	}
	STRING LPAR paramList RPAR body	

	{
		if($1 == INT) std::cout << "It's int" << std::endl;
		std::cout << $3 << "[p] Got function" << std::endl;

		auto returnResource = interpret.vr->getVar("return");

		interpret.fr->addCompositeFunction($3, interpret.getNode($7),interpret.paramGet(), returnResource);
		std::cout << "Registering: '"<< $3 << "' func" << std::endl;
	}

	;
paramList:
	| param
	| paramList COMMA param
	;
param:
     	type STRING 
	{
		auto res = interpret.tr->sharedResource($1);
		interpret.vr->addVar($2,res);
		interpret.paramPush(res);
		
	}
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
		std::cout << "Adding node " << $1 << std::endl;
	}
	statements
/*	| statements statement
	{
		auto id = interpret.topBody();
		auto body = std::static_pointer_cast<Body>(interpret.getNode(id));
		body->stats.push_back(interpret.getNode($2));
		std::cout << "Adding node " << $2 << std::endl;
		std::cout << "Another node " << $1 << std::endl;
	}
*/
;	
statement:
	printStatement {$$ = $1; 
		std::cout << "Adding node " << $1 << std::endl;}
	| variableDefinition {$$ = $1;
		std::cout << "Adding node " << $1 << std::endl;}
	| assignment {$$ = $1;
		std::cout << "Adding node " << $1 << std::endl;}
	| returnStatement
	| whileStatement
	| ifStatement

ifStatement:
	   KEYWORD_IF LPAR expr RPAR  body
	{
		auto ifBox = std::make_shared<If>();
		ifBox->setExpression(interpret.getFunction($3));
		ifBox->setPath(0,interpret.getNode($5));
		ifBox->setPath(1,interpret.getNode(interpret.createNOOP()));
		$$ = interpret.addNode(ifBox);
	}

whileStatement:
	      KEYWORD_WHILE LPAR  expr RPAR body
		{
			auto whileBox = std::make_shared<While>();
			whileBox->expr = interpret.getFunction($3);
			whileBox->stat = interpret.getNode($5);
			$$ = interpret.addNode(whileBox);
	
		}

returnStatement:
	       KEYWORD_RETURN expr SEMICOL
		{
			auto retResource = interpret.vr->getVar("return");
			auto node = interpret.getFunction($2);
			std::cout << retResource->getName() << " omg wtf" << std::endl;
			auto copy = interpret.fr->getFunc(std::string("Copy:")+retResource->getName());
			copy->bindInput(0,node);
			copy->bindOutput(retResource);
			$$ = interpret.addNode(copy);	
		}
printStatement:
	      KEYWORD_PRINT STRING SEMICOL
		{
			auto var = interpret.vr->getVar($2);
			std::cout << "Type" << var->getName() << std::endl;
			auto print = interpret.fr->getFunc(std::string("PrintValue:")+var->getName());
			print->bindInput(0,interpret.fr->getHandler(var));
			$$ = interpret.addNode(print);
		}
while:
     KEYWORD_WHILE LPAR expr RPAR LBRAC statements RBRAC
assignment:
	STRING KEYWORD_ASSIGN expr SEMICOL
	{
		$$ = interpret.createAssignment($1,$3);
	}

variableDefinition:
	type STRING SEMICOL
	{
		auto res = interpret.tr->sharedResource($1);
		interpret.vr->addVar(std::string($2),res);
		$$ = interpret.createNOOP();
	}
	| type STRING KEYWORD_ASSIGN expr SEMICOL 
	{
		auto res = interpret.tr->sharedResource($1);
		interpret.vr->addVar(std::string($2),res);
		$$ = interpret.createAssignment($2,$4);
	}
expr:
   	exprConst
	| expr MUL expr {$$ = interpret.createOperation("Mul", $1, $3);}
	| expr DIV expr {$$ = interpret.createOperation("Div", $1, $3);}
	| expr PLUS expr {$$ = interpret.createOperation("Add", $1, $3);}
	| expr MINUS expr{$$ = interpret.createOperation("Sub", $1, $3);}
	| expr GREATER expr {$$ =interpret.createOperation("Greater", $1, $3);}
	| expr LESS expr {$$ = interpret.createOperation("Less", $1, $3);}
	| expr EQ expr {$$ = interpret.createOperation("Eq", $1, $3);}

exprConst:
	 INT {$$ = interpret.createResource($1);}
	| FLOAT {$$ = interpret.createResource($1);}
	| STRING {interpret.addLastString($1); std::cout << "LOL" << std::endl; } functionCall {std::cout << "The end" << std::endl; $$ = $3;} 
	| STRING LANGL INT RANGL {}
	| LPAR expr RPAR {$$ = $2;} 
	
functionCall:
	{
		$$ = interpret.addNode(interpret.fr->getHandler(interpret.vr->getVar(interpret.getLastString())));
		interpret.popLastString();
	}
	| 

	LPAR expr RPAR
	{
		auto func = interpret.fr->getFunc(interpret.getLastString());	
		interpret.popLastString();
		func->bindInput(0, interpret.getFunction($2));
		$$ = interpret.addNode(func);
	}


/*exprParams:
	  expr 
	  expr ex
	| exprParams expr

*/
type:
    	KEYWORD_INT {$$ = interpret.tr->getTypeId("int");}
    	| KEYWORD_FLOAT {$$ = interpret.tr->getTypeId("float");}
	| KEYWORD_CHAR {$$ = interpret.tr->getTypeId("char");}
	| KEYWORD_BOOL {$$ = interpret.tr->getTypeId("bool");}
    	| STRING {$$ = interpret.tr->getTypeId($1);}
	;
;
%%

int main(int argc, char** argv) {
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


