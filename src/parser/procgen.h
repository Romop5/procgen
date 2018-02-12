/*
 * Compilation helper structs / classes
 */
#ifndef PROCGEN_H
#define PROCGEN_H

#include "derivation.h"

#include <string>
#include <stack>
#include "json.hpp"

namespace ProcGen {
	class sTypeDeclaration {
		public:
		std::shared_ptr<Resource>   resource;
		std::string                 name;
	};

    struct Rule {
        std::shared_ptr<Resource>   conditionReturn;
        TypeId                      thisTypeId;
	std::shared_ptr<Resource>   thisValue;
    };

	class Generation {
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
		bool parseFile(const std::string& file);

        // Run inicialization
        bool runInit();

        // Run real derivation
        bool run(int maximumSteps);

        json serialize() const;

		bool isReady()	{ return flagIsParsed; }

        // Error report
        bool errorMessage(const char* message, ...);

        void setDebugOn(bool state);
		/*
		 *	Utility functions, called by parser
		 */

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

		bool createFunctionCall(const char* functionName);
				

		// Replace expression with argument
		bool createArgument();


		bool registerLocalVariable(const char* type, const char* name,bool hasExp);

        bool makeReturn(bool hasExpression);
		
		/* Compilation utilities*/
		std::vector<sTypeDeclaration> typeList;

		std::stack<std::shared_ptr<Function>> expressionsStack;


		// Function argument list 
		std::vector<std::shared_ptr<Function>> argumentVector;

		bool makeAssignment(const char* name,bool hasAssignment);

		bool makeWhile();

		bool makeIfStatement(bool hasElseBranch);

        bool makeCallStatement();

		bool pushBody();
		std::shared_ptr<Body> popBody();
		// This stacked structure is used to handle N-anry 
		std::stack<std::shared_ptr<Body>> stackedBodies;
    
        bool hasType(const char* name);
        
        bool registerFormalParameter(sTypeDeclaration& parameter);

        sTypeDeclaration fillTypeDeclaration(char* type, char* name);

        bool initializeRule(char* typeName);
        bool ruleProcedure(char* rulename);
        
        Rule ruleDefinition;

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
