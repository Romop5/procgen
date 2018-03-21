#include <procgen/interpret/types.h>
#include <procgen/interpret/typereg.h>
#include <procgen/interpret/functionreg.h>
#include <procgen/interpret/compositeutils.h>

#include <set>

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
	#define REG_FUNC(type,func)\
		fr->addFunction(#func ":" #type,\
		[]{return std::static_pointer_cast<Function>(std::make_shared<func<type>>());});

	#define REG_FUNC2(type,type2,func)\
		fr->addFunction(#func ":" #type ":" #type2,\
		[]{return std::static_pointer_cast<Function>(std::make_shared<func<type,type2>>());});



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

	REG_FUNC(bool, Negation );
	REG_FUNC(int, Modulo);

	REG_FUNC2(float,int, Cast);
	REG_FUNC2(int,float, Cast);
	REG_FUNC2(bool,int, Cast);

	REG_FUNC_FORALL("And", And);
	REG_FUNC_FORALL("Or",Or);
	REG_FUNC_FORALL("Add",Add);
	REG_FUNC_FORALL("Sub",Sub);
	REG_FUNC_FORALL("Mul",Mul);
	REG_FUNC_FORALL("Eq",Eq);
	REG_FUNC_FORALL("NotEq",NotEq);
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

bool getCommonType(std::string first, std::string second)
{
    // FROM -> TO
    static std::set<std::pair<std::string,std::string>> convertibleRelation = 
        {
            // int to float is ok
            {std::make_pair("int","float")},
            // int to bool is ok
            {std::make_pair("int","bool")},
        };

    if(convertibleRelation.find(std::make_pair(first, second)) != convertibleRelation.end())
    {
        return 2;
    }

    if(convertibleRelation.find(std::make_pair(second, first)) != convertibleRelation.end())
    {
        return 1;
    }
    return NULL;
}
