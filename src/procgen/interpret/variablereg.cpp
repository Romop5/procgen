#include <procgen/interpret/variablereg.h>
#include <procgen/utils/logger.h>
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
		LOG_DEBUG("VR: %s\n",x.first.c_str()); 
	}
}

void VariableReg::clear()
{
	this->vars.clear();
}
