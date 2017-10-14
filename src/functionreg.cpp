#include "functionreg.h"
#include <iostream>

bool FunctionReg::addFunction(std::string name,func_constr c)
{
	this->func[name] = c;	
}

std::shared_ptr<Function> FunctionReg::getFunc(std::string name)
{
	auto it = this->func.find(name);
	if(it != this->func.end())
	{
		return this->func[name]();
	}
	return nullptr;
}

// Print out all info
void FunctionReg::_debug()
{
	for(auto &x: this->func)
	{
		std::cout << x.first << std::endl;
	}

}
/*
template<class X>
bool FunctionReg::addFunction(std::string name)
{
	// create a constructor for class
	this->addFunction(name, []{return std::static_pointer_cast<Function>(std::make_shared<X>());});
}
*/
