/*
 * Compilation helper structs / classes
 */
#ifndef PROCGEN_H
#define PROCGEN_H

#include <procgen/derivation/derivation.h>
#include <procgen/parser/utils.h>
#include <procgen/parser/scanner.h>

#include "location.hh"

#include <string>
#include <stack>
#include "json.hpp"

namespace ProcGen {
    
    // fwd dec
    class Parser;
    class Scanner;
    class Location;

	class Generation {
        private:
        
        Scanner* _scanner;        
        Parser*  _parser;        
        location _location;

        friend class Parser;
        friend class Scanner;

		std::shared_ptr<TypeRegister>	typeregister;
		std::shared_ptr<FunctionReg>	functionregister;
		// Stores all local variables (local to function, rule)
		std::shared_ptr<VariableReg> 	localStackFrame;
		std::shared_ptr<VariableReg> 	globalVariables;
		std::shared_ptr<Derivation> 	der;
		bool	flagIsParsed;
        bool    hasAnyError;
		public:
		Generation();
        ~Generation();

		/* Compilation utilities*/
		std::vector<sTypeDeclaration> typeList;
		std::stack<std::shared_ptr<Function>> expressionsStack;

		// This stacked structure is used to handle N-anry 
		BodyStack stackedBodies;

		// Function argument list 
        ArgumentVector argumentVector;
        

        Rule ruleDefinition;

		bool parseFile(const std::string& file);

        // Run inicialization
        bool runInit();

        // Run real derivation
        bool run(int maximumSteps);

        json serialize() const;

		bool isReady()	{ return flagIsParsed; }

        // Error report
        bool errorMessage(std::string message, ...);

        void setDebugOn(bool state);
		/*
		 *	Utility functions, called by parser
		 */

        void registerNatives();
        // Prepare local stack
        bool initializeFunction(char* type);

		bool registerRule(char* name,char* type);

		// Register a new name for type.
		// May fail on missing aliasedType or alias name collision
		bool registerAlias(char* alias, char* aliasedType);

		// Define a structure with its members. May fail on invalid type / name duplication
		bool registerStruct(char* name, std::vector<sTypeDeclaration>& types);

		// Register global parameter
		bool registerParameter(char* name, char* type,bool hasLiteral);

		bool registerFunction(char* type, char* name);

		std::shared_ptr<Function> createUnaryOperation(char operation);
		std::shared_ptr<Function> createExpressionOperation(char operation);
		std::shared_ptr<Function> createExpressionLogicOperation(char operation);

		void createLiteralBool(bool value);
		void createLiteralInteger(int value);
		void createLiteralFloat(float value);
        bool createLiteralFromVariable(char* name); 
        bool createStructuredLiteral(char* member); 

		bool createFunctionCall(const char* functionName,std::vector<Argument>);
				

		// Replace expression with argument
		bool createArgument();
		bool registerLocalVariable(const char* type, const char* name,bool hasExp);
        bool makeReturn(bool hasExpression);
		



		bool makeAssignment(const char* name,bool hasAssignment);

		bool makeWhile();

		bool makeIfStatement(bool hasElseBranch);

        bool makeCallStatement();

    
        bool hasType(const char* name);
        
        bool registerFormalParameter(sTypeDeclaration& parameter);

        sTypeDeclaration fillTypeDeclaration(char* type, char* name);

        bool initializeRule(char* typeName);
        bool ruleProcedure(char* rulename);
        

	bool makeTypeid(char* name);
	bool makeConvert(char* name);

	bool createCollectionInsert();
	bool createCollectionAt();
	bool createCollectionSize();
	bool createCollectionDel();

	bool isLogicOperator(char operatorName)
	{
		switch(operatorName)
		{
			case '>':
			case '<':
			case '=':
			case '!':
				return true;
			default:
				return false;
		}
		return false;
	}
	};


}
#endif
