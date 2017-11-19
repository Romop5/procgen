#include "derivation.h"

std::shared_ptr<Function> getPredicate()
{
	
}
int main()
{

	
	auto tr = std::make_shared<TypeReg>();
	auto fr = std::make_shared<FunctionReg>(tr);
	
	registerStandardTypes(tr.get());
	registerStandardFunctions(fr.get());

	Derivation derivation(tr,fr);


	return 0;
}
