#include <procgen/interpret/function.h>
bool Function::operator()(RunStatus& stat)
{
	if(_doInputs(stat)) return true;
	return false;
}

bool Function::bindInput(size_t id, std::shared_ptr<Function> func)
{
	this->_inputs[id] = func;	
    return true;
}
bool Function::bindOutput(std::shared_ptr<Resource> res)
{
	this->_output = res;
    return true;
}

bool Function::_doInputs(RunStatus& stat)
{
	for(auto &x: this->_inputs)
	{
		if((*(x.second))(stat) == true)
        {
            LOG_DEBUG("Return triggered, coming home.\n");
			return true;
        }
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
