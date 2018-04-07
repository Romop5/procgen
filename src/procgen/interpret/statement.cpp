#include <procgen/interpret/statement.h>
#include <procgen/interpret/function.h>
bool If::operator()(RunStatus& stat)
{
	// get result of expression
	if((*expr)(stat)) return true;

	//expr.getOutput().value
	// fixed on 2017/12/15
	bool first = *(bool*) expr->getOutput()->getData();
	// evaluate
	if(first)
		return (*paths[0])(stat);
	else
		return (*paths[1])(stat);
}
void If::setExpression(std::shared_ptr<Function> exp)
{
	this->expr = exp;
}

void If::setPath(size_t id, std::shared_ptr<Statement> path)
{
	this->paths[id] = path;
}


json If::to_json() const 
{
    json js = json(getBoxName());
    for(auto &path: this->paths)
    {
        js[path.first] = (path.second)->to_json();   
    }
    return js;
}

bool While::operator()(RunStatus& stat)
{
	while(true)
	{
		if((*this->_expression)(stat)) return true;
		bool isTrue = *(bool*) this->_expression->getOutput()->getData();
        LOG_DEBUG("While expression result: %d\n", isTrue);
		if(!isTrue)
			break;
		// eval statement
		if((*this->_statement)(stat)) return true;
	}
	return false;
}
bool Body::operator()(RunStatus& stat)
{
	for(auto x: statements)
	{
		if((*x)(stat)) return true;
	}
	return false;
}

bool Return::operator()(RunStatus& stat)
{
    // If provided expression 
    if(this->input != nullptr) 
        (*this->input)(stat);

    stat.setStatus(RunStatus::RETURN_REACHED);
    return true;
}

