#include "typereg.h"
#include <iostream>
#include "variablereg.h"

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
	
}
