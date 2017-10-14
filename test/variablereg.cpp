#include "typereg.h"
#include <iostream>
#include "variablereg.h"
#include "functionreg.h"

using namespace std;
int main()
{
	std::shared_ptr<TypeRegister> tr = std::make_shared<TypeRegister>() ;
	#define REG_TYPE(type,typeName)\
		tr->add<type>(typeName);
	// Register all built int types
	FORALL_ATOMICTYPES(REG_TYPE);
	
	auto vr = VariableReg(tr);
	vr.addVar("a", tr->sharedResource("int"));
	vr._debug();

	auto fr = FunctionReg(tr);	

	#define REG_FUNC_FORALL(funcname)\
		FORALL_ATOMICTYPES(REG_TYPE);
	#define REG_FUNC(type,typeName)\
		fr.addFunction(##funcname#typeName,[]{return std::make_shared<funcname<type>>()});
	
	REG_FUNC_FORALL(tAdd);
	std::cout << "Debug" <<std::endl;
	fr._debug();	
		
}
