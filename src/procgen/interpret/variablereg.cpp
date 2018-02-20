#include <procgen/interpret/variablereg.h>
#include <iostream>
bool VariableReg::addVar(std::string name,std::shared_ptr<Resource> src)
{
	this->vars[name] = src;
}
std::shared_ptr<Resource> VariableReg::getVar(std::string name)
{
	auto it = this->vars.find(name);
	if(it != this->vars.end())
	{
		return this->vars[name];
	}
	return nullptr;
}
		
		// Print out all info
void VariableReg::_debug()
{
	for(auto &x: this->vars)
	{
		std::cout << "VR: " << x.first << std::endl;
	}
}

void VariableReg::clear()
{
	this->vars.clear();
}
