#include <procgen/interpret/function.h>
bool Function::operator()(RunStatus& stat)
{
	if(_doInputs(stat)) return true;
	return false;
}

bool Function::bindInput(size_t id, std::shared_ptr<Function> func)
{
	this->_inputs[id] = func;	
}
bool Function::bindOutput(std::shared_ptr<Resource> res)
{
	this->_output = res;
}

bool Function::_doInputs(RunStatus& stat)
{
	for(auto &x: this->_inputs)
	{
		if((*(x.second))(stat) == true)
			return true;
	}
	return false;
}

bool DebugStatement::operator()(RunStatus& stat)
{
	//TODO
	return false;
}

bool HandleFunction::operator()(RunStatus& stat)
{
	return false;
}
