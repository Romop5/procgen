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

		std::shared_ptr<Function> createExpressionOperation(char operation, 
				std::shared_ptr<Function> first, std::shared_ptr<Function> second);

		
		/* Compilation utilities*/
		std::vector<sTypeDeclaration> typeList;
	};
}
#endif
