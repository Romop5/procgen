%code requires {

#include <procgen/parser/procgen.h>
using namespace ProcGen;

}

%{

#include <cstdio>
#include <iostream>
#include <memory> 
#include <error.h>

#include <procgen/parser/procgen.h>
using namespace ProcGen;
// stuff from flex that bison needs to know about:
//extern "C" int yylex();
extern int yylex(ProcGen::Generation* proc);
//extern "C" int yyparse();
//extern "C" FILE *yyin;


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
%token IF STRUCT USING RULE PARAMETER  ELSE WHILE RETURN CONVERT TYPEID SIZE AT DEL INSERT
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
%token NOTEQ   "!="
%token MINUS    "-"
%token PLUS     "+"
%token DIV      "/"
%token MUL      "*"
%token DOT      "."

%token GREATER ">"
%token LESS    "<"
// Operators
%left EQ NOTEQ
%left GREATER LESS

%left "-" 
%left "+" 
%left "/" 
%left "*" 

%left UMINUS  
%left UPLUS 
%left NEGATION "!"


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
                    |   RULE TYPE { proc->initializeRule($2); } compoundStatement 
                        { proc->ruleProcedure($2); } compoundStatement 
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
			 {sTypeDeclaration temp = proc->fillTypeDeclaration($1,$2);
              proc->typeList.push_back(temp);
              proc->registerFormalParameter(temp);
              //std::cout << "typeList " << proc->typeList.size() << std::endl;
             }



compoundStatement	  : "{" {proc->stackedBodies.pushBody();} statements "}" 

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
functionCall             : NAME 
             { proc->argumentVector.pushArgumentLevel(); }
                           "(" argumentList ")" 
			 {  auto args = proc->argumentVector.popArgumentLevel();
                proc->createFunctionCall($1,args); }

argumentList             : %empty | argument | argumentList "," argument

argument                  : expression
			  { proc->createArgument(); }

assignment                : structuredMember  assignmentEnd
assignmentEnd		  : %empty
			  { proc->makeAssignment("TODO",false); }
		 	   | "=" expression
			  { proc->makeAssignment("TODO",true); }


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


typeid			  : "<" TYPE ">" 
				{ proc->makeTypeid($2);}
	   		  | "(" NAME ")" 
				{ proc->makeTypeid($2);}
expression                : literal
			    |   TYPEID typeid
			    |   CONVERT "<" TYPE ">" "(" expression ")"
				{ proc->makeConvert($3); }
			    | 	functionCall 
			  
                            |   expression "<" expression
				{ auto result = proc->createExpressionOperation('<'); }
                            |   expression ">" expression
				{ auto result = proc->createExpressionOperation('>'); }
                            |   expression "==" expression
				{ auto result = proc->createExpressionOperation('='); }
                            |   expression "!=" expression
				{ auto result = proc->createExpressionOperation('!'); }
                            |   expression "-" expression
				{ auto result = proc->createExpressionOperation('-'); }
                            |   expression "+" expression
				{ auto result = proc->createExpressionOperation('+'); }
                            |   expression "/" expression
				{ auto result = proc->createExpressionOperation('/'); }
                            |   expression "*" expression
				{ auto result = proc->createExpressionOperation('*'); }
                            |   "(" expression ")" 
		            |   "-" expression %prec UMINUS
				{ auto result = proc->createUnaryOperation('-'); }
		            |   "+" expression %prec UPLUS
		            |   "!" expression 
				{ auto result = proc->createUnaryOperation('!'); }


structuredMember   :   structuredMember "." structuredMemberEnd 
                   |    NAME 
                        { proc->createLiteralFromVariable($1); } 

structuredMemberEnd :
			NAME
                        { proc->createStructuredLiteral($1); }
		   |	INSERT "(" expression ")"
			{ proc->createCollectionInsert(); } 
		   |	AT "(" expression ")"
			{ proc->createCollectionAt(); } 
		   |	SIZE "(" ")"
			{ proc->createCollectionSize(); } 
		   |    DEL "(" expression ")"
			{ proc->createCollectionDel(); } 

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
void yyerror(Generation* proc, const char *s) {
    proc->errorMessage(s);
	//std::cout << s << " at line: "<< yylloc.first_line << ":" << yylloc.first_column <<  std::endl;
	//exit(-1);
}

