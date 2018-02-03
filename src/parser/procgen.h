/*
 * Compilation helper structs / classes
 */
#ifndef PROCGEN_H
#define PROCGEN_H

#include "derivation.h"

#include <string>
#include <stack>

namespace ProcGen {
	
	class sTypeDeclaration {
		public:
		sTypeDeclaration(char* type, char* name)
			: mType(type), mName(name) {}
		std::string mType;
		std::string mName;
	};

	class Generation {
		std::shared_ptr<TypeRegister>	typeregister;
		std::shared_ptr<FunctionReg>	functionregister;
		// Stores all local variables (local to function, rule)
		std::shared_ptr<VariableReg> 	localStackFrame;
		std::shared_ptr<VariableReg> 	globalVariables;
		std::shared_ptr<Derivation> 	der;
		bool	flagIsParsed;
		public:
		Generation();
		bool parseFile(const std::string& file);

		bool isReady()	{ return flagIsParsed; }

		/*
		 *	Utility functions, called by parser
		 */

		// Register a new name for type.
		// May fail on missing aliasedType or alias name collision
		bool registerAlias(char* alias, char* aliasedType);

		// Define a structure with its members. May fail on invalid type / name duplication
		bool registerStruct(char* name, std::vector<sTypeDeclaration>& types);

		// Register global parameter
		bool registerParameter(char* name, char* type);

		bool registerFunction(char* type, char* name);

		std::shared_ptr<Function> createExpressionOperation(char operation);

		void createLiteralInteger(int value);
		void createLiteralFloat(float value);

		bool createFunctionCall(const char* functionName);
				

		// Replace expression with argument
		bool createArgument();


		bool registerLocalVariable(const char* type, const char* name);
		
		/* Compilation utilities*/
		std::vector<sTypeDeclaration> typeList;

		std::stack<std::shared_ptr<Function>> expressionsStack;


		// Function argument list 
		std::vector<std::shared_ptr<Function>> argumentVector;

		bool makeAssignment(const char* name);

		bool makeWhile();

		bool makeIfStatement(bool hasElseBranch);

		bool pushBody();
		std::shared_ptr<Body> popBody();
		// This stacked structure is used to handle N-anry 
		std::stack<std::shared_ptr<Body>> stackedBodies;
	};
}
#endif
