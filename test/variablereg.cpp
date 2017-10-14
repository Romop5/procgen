#include "typereg.h"
#include <iostream>
#include "variablereg.h"
#include "functionreg.h"
#include "std.h"

using namespace std;
int main()
{
	std::shared_ptr<TypeRegister> tr = std::make_shared<TypeRegister>() ;
	/*#define REG_TYPE(type,typeName)\
		tr->add<type>(typeName);
	// Register all built int types
	FORALL_ATOMICTYPES(REG_TYPE);
	*/
	
	registerStandardTypes(&(*tr));

	auto vr = VariableReg(tr);
	vr.addVar("a", tr->sharedResource("int"));
	vr._debug();

	auto fr = FunctionReg(tr);	

	/*#define REG_FUNC_FORALL(funcname,func)\
		FORALL_ATOMICTYPES3(REG_FUNC,func);\
		;
	#define REG_FUNC(type,typeName,func)\
		fr.addFunction(#func ":" typeName,\
		[]{return std::static_pointer_cast<Function>(std::make_shared<func<type>>());});

	
	REG_FUNC_FORALL("tAdd",tAdd);
	*/
	
	registerStandardFunctions(&fr);
	//fr.addFunction("addint",[](){return std::static_pointer_cast<Function> (std::make_shared<tAdd<int>>());});
	std::cout << "Debug" <<std::endl;
	//fr._debug();	
		

	auto box = fr.getFunc("tMul:int");
	
	vr.addVar("a", tr->sharedResource("int"));
	vr.addVar("b", tr->sharedResource("int"));
	vr.addVar("result", tr->sharedResource("int"));

	auto a = vr.getVar("a");
	auto b = vr.getVar("b");
	auto result = vr.getVar("result");

	*(int*) a->getData() = 100;
	*(int*) b->getData() = 5;

	box->inputs.push_back(a);
	box->inputs.push_back(b);
	box->output = result;

	(*box)();

	int r = *(int*) result->getData();
	std::cout << "result: " << r << std::endl;
	
	
}
