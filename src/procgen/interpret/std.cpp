#include <procgen/interpret/types.h>
#include <procgen/interpret/typereg.h>
#include <procgen/interpret/functionreg.h>
#include <procgen/interpret/compositeutils.h>

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

/*	#define REG_PRINT(type,typeName)\
		fr->addFunction("PrintValue:" typeName,\
		[]{return std::static_pointer_cast<Function>(std::make_shared<PrintValue<type>>());});
*/
    
    
	//FORALL_ATOMICTYPES(REG_PRINT);

	REG_FUNC_FORALL("Copy",Copy);
	REG_FUNC_FORALL("UnaryMinus",UnaryMinus);
	REG_FUNC_FORALL("UnaryPlus",UnaryPlus);

	REG_FUNC(bool, "Negation", Negation );

	REG_FUNC_FORALL("Add",Add);
	REG_FUNC_FORALL("Sub",Sub);
	REG_FUNC_FORALL("Mul",Mul);
	REG_FUNC_FORALL("Eq",Eq);
	REG_FUNC_FORALL("Greater",Greater);
	REG_FUNC_FORALL("Less",Less);
	REG_FUNC_FORALL("Div",Div);

	// composite utils
	
	REG_REGULAR_FUNC("get",AccessComposite);
	REG_REGULAR_FUNC("set",SetComposite);

	fr->addFunction("random",
	[tr]{
		
		auto generate = std::static_pointer_cast<Function>(std::make_shared<GenerateRandom>());
		auto result = tr->sharedResource("float");
		generate->bindOutput(result);
		return generate;
	});

	fr->addFunction("setRandomSeed",
	[tr]{
		
		auto generate = std::static_pointer_cast<Function>(std::make_shared<SetRandomSeed>());
		auto result = tr->sharedResource("float");
		generate->bindOutput(result);
		return generate;
	});



	fr->addFunction("uniform",
	[tr]{
		
		auto generate = std::static_pointer_cast<Function>(std::make_shared<GenerateUniform>());
		auto result = tr->sharedResource("float");
		generate->bindOutput(result);
		return generate;
	});

    fr->addFunction("print",
    []{return std::static_pointer_cast<Function>(std::make_shared<PrintJson>());});

}
