%code requires {


    namespace ProcGen
    {
        class Generation;
        
        inline void yyerror(const char* msg)
        {
        }
    }

}

%{

#include <cstdio>
#include <iostream>
#include <memory> 
#include <procgen/parser/generation.h>
#include <location.hh>
#include <position.hh>

#define yylex generation._scanner->yylex
%}


%require "2.4"
%language "C++"
%locations
%defines
%debug 
%define api.namespace {ProcGen}
%define parser_class_name {Parser}
%parse-param {Generation& generation}
%lex-param {Generation& generation}
%error-verbose

%union {
	int     ival;
	float   fval;
	const char *sval;
	size_t  nodeId;
    bool    boolean;
}

%token TOK_EOF 0
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
%token MODULO   "%"
%token EQ       "=="
%token AND      "&&"
%token OR       "||"
%token NOTEQ   "!="
%token MINUS    "-"
%token PLUS     "+"
%token DIV      "/"
%token MUL      "*"
%token DOT      "."
%token ADDEQ    "+="
%token MULEQ    "*="
%token MINEQ    "-="
%token DIVEQ    "/="

%token GREATER ">"
%token LESS    "<"
// Operators
%left AND OR
%left EQ NOTEQ
%left GREATER LESS

%left "+" "-" 
%left "%" 
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
		   	{ generation.registerStruct($<sval>-1, generation.typeList); }
                    |   TYPE
			{ generation.registerAlias($<sval>-1, $1);}
                    |   RULE TYPE { generation.initializeRule($2); } compoundStatement 
                        { generation.ruleProcedure($2); } compoundStatement 
			{ generation.registerRule($<sval>-1, $2); }


parameterDeclaration  : PARAMETER TYPE NAME assign

assign                 : "=" literal ";"
		      	{ generation.registerParameter($<sval>0,$<sval>-1,true);}
			 | ";" 
		      	{ generation.registerParameter($<sval>0,$<sval>-1,false);}


functionDeclaration   : TYPE NAME { generation.initializeFunction($1);} "(" typeList ")" compoundStatement 
		      	{ generation.registerFunction($1,$2);}


structureDeclaration     : typeDeclaration ";" | structureDeclaration typeDeclaration ";" 


typeList                 : typeDeclaration | typeList "," typeDeclaration
                            | %empty

typeDeclaration          : TYPE NAME 
			 {sTypeDeclaration temp = generation.fillTypeDeclaration($1,$2);
              generation.typeList.push_back(temp);
              generation.registerFormalParameter(temp);
              //std::cout << "typeList " << generation.typeList.size() << std::endl;
             }



compoundStatement	  : "{" {generation.stackedBodies.pushBody();} statements "}" 

statements                : statement statements | %empty 

statement                 : callStatement ";" | declaration ";" | assignment ";"
                                | ifStatement  | whileStatement | return ";"

declaration               : TYPE NAME declarationEnd

declarationEnd           :      %empty 
			  { generation.registerLocalVariable($<sval>-1,$<sval>0,false); }
                            |   "=" expression 
			  { generation.registerLocalVariable($<sval>-1,$<sval>0,true); }

callStatement           : functionCall
                         { generation.makeCallStatement(); } 
functionCall             : NAME 
             { generation.argumentVector.pushArgumentLevel(); }
                           "(" argumentList ")" 
			 {  auto args = generation.argumentVector.popArgumentLevel();
                generation.createFunctionCall($1,args); }

argumentList             : %empty | argument | argumentList "," argument

argument                  : expression
			  { generation.createArgument(); }

assignment                : structuredMember  assignmentEnd
assignmentEnd		  : %empty
			  { generation.makeAssignment("TODO",false); }
		 	   | "=" expression
			  { generation.makeAssignment("TODO",true); }

		 	   | "+=" expression
			  { generation.makeAssignment("TODO",true,'+'); }

		 	   | "-=" expression
			  { generation.makeAssignment("TODO",true,'-'); }

		 	   | "*=" expression
			  { generation.makeAssignment("TODO",true,'*'); }

		 	   | "/=" expression
			  { generation.makeAssignment("TODO",true,'/'); }


ifStatement              : IF "(" expression ")" compoundStatement elseClause
elseClause               : ELSE compoundStatement 
			  { generation.makeIfStatement(true);}
			  | %empty
			  { generation.makeIfStatement(false);}

whileStatement           : WHILE "(" expression ")" compoundStatement
			  { generation.makeWhile();}

return                  :  RETURN returnEnd 

returnEnd               :  %empty 
                        { generation.makeReturn(false); } 
                        |  expression
                        { generation.makeReturn(true); } 


typeid			  : "<" TYPE ">" 
				{ generation.makeTypeid($2);}
	   		  | "(" NAME ")" 
				{ generation.makeTypeid($2);}
expression                : literal
			    |   TYPEID typeid
                |    TYPE 
                    { generation.argumentVector.pushArgumentLevel(); }
                           "(" argumentList ")" 
                    {  auto args = generation.argumentVector.popArgumentLevel();
                        generation.makeConstructor($1,args); }


                |   "(" TYPE ")" "(" expression ")" 
                    { generation.makeExplicitCast($2); }
			    |   CONVERT "<" TYPE ">" "(" expression ")"
                    { generation.makeConvert($3); }
			    | 	functionCall 
			  
                            |   expression "&&" expression
				{ auto result = generation.createExpressionOperation('&'); }
                            |   expression "||" expression
				{ auto result = generation.createExpressionOperation('|'); }
                            |   expression "<" expression
				{ auto result = generation.createExpressionOperation('<'); }
                            |   expression ">" expression
				{ auto result = generation.createExpressionOperation('>'); }
                            |   expression "==" expression
				{ auto result = generation.createExpressionOperation('='); }
                            |   expression "!=" expression
				{ auto result = generation.createExpressionOperation('!'); }
                            |   expression "-" expression
				{ auto result = generation.createExpressionOperation('-'); }
                            |   expression "+" expression
				{ auto result = generation.createExpressionOperation('+'); }
                            |   expression "/" expression
				{ auto result = generation.createExpressionOperation('/'); }
                            |   expression "*" expression
				{ auto result = generation.createExpressionOperation('*'); }
                            |   expression "%" expression
				{ auto result = generation.createExpressionOperation('%'); }
                            |   "(" expression ")" 
		            |   "-" expression %prec UMINUS
				{ auto result = generation.createUnaryOperation('-'); }
		            |   "+" expression %prec UPLUS
		            |   "!" expression %prec NEGATION
				{ auto result = generation.createUnaryOperation('!'); }


structuredMember   :   structuredMember "." structuredMemberEnd 
                   |    NAME 
                        { generation.createLiteralFromVariable($1); } 

structuredMemberEnd :
			NAME
                        { generation.createStructuredLiteral($1); }
		   |	INSERT "(" expression ")"
			{ generation.createCollectionInsert(); } 
		   |	AT "(" expression ")"
			{ generation.createCollectionAt(); } 
		   |	SIZE "(" ")"
			{ generation.createCollectionSize(); } 
		   |    DEL "(" expression ")"
			{ generation.createCollectionDel(); } 

/* Terminals*/

literal                   : INTEGER 
				{ generation.createLiteralInteger($1); } 
			  | FLOAT
				{ generation.createLiteralFloat($1); } 
			  | STRING 
              | BOOL
				{ generation.createLiteralBool($1); } 
              | structuredMember
                

%%

namespace ProcGen
{
    void Parser::error(const location& loc, const std::string& m)
    {
        generation.errorMessage(m);
    }
}
