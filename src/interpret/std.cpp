#include "types.h"
#include "typereg.h"
#include "functionreg.h"
#include "compositeutils.h"

void registerStandardTypes(TypeRegister* tr)
{
	#define REG_TYPE(type,typeName)\
		tr->add<type>(typeName);
	// Register all built int types
	FORALL_ATOMICTYPES(REG_TYPE);
}

void registerStandardFunctions(FunctionReg* fr)
{
	std::shared_ptr<TypeRegister> tr = fr->getTypeRegister();
	#define REG_FUNC_FORALL(funcname,func)\
		FORALL_ATOMICTYPES3(REG_FUNC,func);\
		;
	#define REG_FUNC(type,typeName,func)\
		fr->addFunction(#func ":" typeName,\
		[]{return std::static_pointer_cast<Function>(std::make_shared<func<type>>());});

	#define REG_REGULAR_FUNC(funcName,func)\
		fr->addFunction(std::string(funcName),\
		[tr]{return std::static_pointer_cast<Function>(std::make_shared<func>(tr));});
		//[&]{return std::static_pointer_cast<Function>(std::make_shared<func>(tr));});


	REG_FUNC_FORALL("tCopy",tCopy);

	REG_FUNC_FORALL("tAdd",tAdd);
	REG_FUNC_FORALL("tSub",tSub);
	REG_FUNC_FORALL("tMul",tMul);
	REG_FUNC_FORALL("tEq",tEq);
	REG_FUNC_FORALL("tGreater",tGreater);
	REG_FUNC_FORALL("tDiv",tDiv);

	// composite utils
	
	REG_REGULAR_FUNC("get",AccessComposite);
	REG_REGULAR_FUNC("set",SetComposite);
}
