#include "types.h"
#include "typereg.h"
#include "functionreg.h"
void registerStandardTypes(TypeRegister* tr)
{
	#define REG_TYPE(type,typeName)\
		tr->add<type>(typeName);
	// Register all built int types
	FORALL_ATOMICTYPES(REG_TYPE);
}

void registerStandardFunctions(FunctionReg* fr)
{
	#define REG_FUNC_FORALL(funcname,func)\
		FORALL_ATOMICTYPES3(REG_FUNC,func);\
		;
	#define REG_FUNC(type,typeName,func)\
		fr->addFunction(#func ":" typeName,\
		[]{return std::static_pointer_cast<Function>(std::make_shared<func<type>>());});

	
	REG_FUNC_FORALL("tAdd",tAdd);
	REG_FUNC_FORALL("tMul",tMul);
	//REG_FUNC_FORALL("tEq",tEq);
	REG_FUNC_FORALL("tDiv",tDiv);
}
