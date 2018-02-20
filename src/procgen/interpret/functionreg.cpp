#include <procgen/interpret/functionreg.h>
#include <iostream>

bool FunctionReg::addFunction(std::string name,std::function<std::shared_ptr<Function>()> c)
{
    // if there isn't any function with name @name
    if(this->func.find(name) == func.end())
    {
        this->func[name] = c;	
        return false;
    }
    return false;
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

void FunctionReg::_debug()
{
	for(auto &x: this->func)
	{
		std::cout << x.first << std::endl;
	}

}

