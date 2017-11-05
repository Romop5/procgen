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


	REG_FUNC_FORALL("Copy",Copy);

	REG_FUNC_FORALL("Add",Add);
	REG_FUNC_FORALL("Sub",Sub);
	REG_FUNC_FORALL("Mul",Mul);
	REG_FUNC_FORALL("Eq",Eq);
	REG_FUNC_FORALL("Greater",Greater);
	REG_FUNC_FORALL("Div",Div);

	// composite utils
	
	REG_REGULAR_FUNC("get",AccessComposite);
	REG_REGULAR_FUNC("set",SetComposite);
}
