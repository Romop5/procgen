#include "typereg.h"
/*#define REG_TYPE(type,typeName)\
		tr->add<type>(typeName);
	// Register all built int types
	FORALL_ATOMICTYPES(REG_TYPE);
	*/
	
void registerStandardTypes(TypeRegister* tr);

void registerStandardFunctions(FunctionReg* fr);

