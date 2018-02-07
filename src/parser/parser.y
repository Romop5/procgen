%code requires {

#include "procgen.h"
using namespace ProcGen;

}

%{

#include <cstdio>
#include <iostream>
#include <memory> 
#include <error.h>

#include "procgen.h"
using namespace ProcGen;
// stuff from flex that bison needs to know about:
//extern "C" int yylex();
extern int yylex(ProcGen::Generation* proc);
//extern "C" int yyparse();
//extern "C" FILE *yyin;


const std::string typeToString(size_t type);
//void yyerror(Interpret& interpret, const char *s);
void yyerror(Generation* proc, const char *s);

%}



%debug 

%define parse.error verbose

%parse-param {Generation* proc}
%lex-param {proc}

%union {
	int     ival;
	float   fval;
	char    *sval;
	size_t  nodeId;
    bool    boolean;
}

%token <ival> INTEGER CHAR 
%token <fval> FLOAT 
%token <boolean> BOOL
%token <sval> STRING

%token <sval> TYPE
%token <sval> NAME 

/* Keywords */
%token IF STRUCT USING RULE PARAMETER  ELSE WHILE RETURN
%token LPAR     "("
%token RPAR     ")"
%token LANGL    "["
%token RANGL    "]"
%token LBRAC    "{"
%token RBRAC    "}"
%token SEMICOL  ";"
%token COMMA    ","
%token ASSIGN	"="
%token EQ       "=="
%token MINUS    "-"
%token PLUS     "+"
%token DIV      "/"
%token MUL      "*"
%token DOT      "."
// Operators
%left "=" 
%left GREATER LESS

%left "-" 
%left "+" 
%left "/" 
%left "*" 


/*%parse-param {Interpret& interpret}*/
%locations

%%
/* Program is a collection of declarations*/
program       : declarations
declarations  : declaration declarations | %empty

/*A declaration is either a using declaration, param declaration, or function declaration*/
declaration   : usingDeclaration | parameterDeclaration | functionDeclaration

/* Using declaration either declares a new type, type alias or defines a rules*/
usingDeclaration : USING NAME "=" usingVariant ";" 

usingVariant        : 	STRUCT "{" structureDeclaration "}"
		   	{ proc->registerStruct($<sval>-1, proc->typeList); }
                    |   TYPE
			{ proc->registerAlias($<sval>-1, $1);}
                    |   RULE TYPE { proc->initializeFunction("bool"); } compoundStatement compoundStatement 
			{ proc->registerRule($<sval>-1, $2); }


parameterDeclaration  : PARAMETER TYPE NAME assign

assign                 : "=" literal ";"
		      	{ proc->registerParameter($<sval>0,$<sval>-1,true);}
			 | ";" 
		      	{ proc->registerParameter($<sval>0,$<sval>-1,false);}


functionDeclaration   : TYPE NAME { proc->initializeFunction($1);} "(" typeList ")" compoundStatement 
		      	{ proc->registerFunction($1,$2);}


structureDeclaration     : typeDeclaration ";" | structureDeclaration typeDeclaration ";" 


typeList                 : typeDeclaration | typeList "," typeDeclaration
                            | %empty

typeDeclaration          : TYPE NAME 
			 {proc->typeList.push_back(sTypeDeclaration($1,$2));}


compoundStatement	  : "{" {proc->pushBody();} statements "}" 

statements                : statement statements | %empty 

statement                 : callStatement ";" | declaration ";" | assignment ";"
                                | ifStatement  | whileStatement | return ";"

declaration               : TYPE NAME declarationEnd

declarationEnd           :      %empty 
			  { proc->registerLocalVariable($<sval>-1,$<sval>0,false); }
                            |   "=" expression 
			  { proc->registerLocalVariable($<sval>-1,$<sval>0,true); }

callStatement           : functionCall
                         { proc->makeCallStatement(); } 
functionCall             : NAME "(" argumentList ")" 
			 { proc->createFunctionCall($1); }

argumentList             : argument | argumentList "," argument

argument                  : expression
			  { proc->createArgument(); }

assignment                : structuredMember "=" expression
			  { proc->makeAssignment("TODO"); }


ifStatement              : IF "(" expression ")" compoundStatement elseClause
elseClause               : ELSE compoundStatement 
			  { proc->makeIfStatement(true);}
			  | %empty
			  { proc->makeIfStatement(false);}

whileStatement           : WHILE "(" expression ")" compoundStatement
			  { proc->makeWhile();}

return                  :  RETURN returnEnd 

returnEnd               :  %empty 
                        { proc->makeReturn(false); } 
                        |  expression
                        { proc->makeReturn(true); } 


expression                : literal
			                | 	functionCall 
			  
                            |   expression "-" expression
				{ auto result = proc->createExpressionOperation('-'); }
                            |   expression "+" expression
				{ auto result = proc->createExpressionOperation('+'); }
                            |   expression "/" expression
				{ auto result = proc->createExpressionOperation('/'); }
                            |   expression "*" expression
				{ auto result = proc->createExpressionOperation('*'); }
                            |   "(" expression ")" 
				{  }


structuredMember   :   structuredMember "." NAME
                        { proc->createStructuredLiteral($3); }
                   |    NAME 
                        { proc->createLiteralFromVariable($1); } 

/* Terminals*/

literal                   : INTEGER 
				{ proc->createLiteralInteger($1); } 
			  | FLOAT
				{ proc->createLiteralFloat($1); } 
			  | STRING 
              | BOOL
				{ proc->createLiteralBool($1); } 
              | structuredMember
                

%%

//int main(int argc, char** argv) {
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

// note 
    //yy_scan_buffer("using a: struct { };");
/*    yydebug = 1;
    if(argc < 2)
		error(1,0,"./parser <FILE>");
	yyin = fopen(argv[1],"r");
	if(yyin == NULL)
		error(1,0,"Failed to open file ...");

	auto typereg = std::make_shared<TypeRegister>();
	auto funcreg = std::make_shared<FunctionReg>(typereg);
	auto derivation = std::make_shared<Derivation>(typereg,funcreg);
    do {
        yyparse(derivation.get());
    } while (!feof(yyin));

}
*/
//void yyerror(Interpret& inter, const char *s) {
void yyerror(Generation* proc, const char *s) {
	std::cout << s << " at line: "<< yylloc.first_line << ":" << yylloc.first_column <<  std::endl;
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


