#include "statement.h"
#include "function.h"
bool If::operator()(RunStatus& stat)
{
	// get result of expression
	if((*expr)(stat)) return true;

	//expr.getOutput().value
	bool first = true;
	// evaluate
	if(first)
		return (*paths[0])(stat);
	else
		return (*paths[1])(stat);
}
void If::setExpression(std::shared_ptr<Function> exp)
{
	this->expr = expr;
}

void If::setPath(size_t id, std::shared_ptr<Statement> path)
{
	this->paths[id] = path;
}

bool While::operator()(RunStatus& stat)
{
	while(true)
	{
		if((*this->expr)(stat)) return true;
		bool isTrue = *(bool*) expr->getOutput()->value;
		if(!isTrue)
			break;
		// eval statement
		if((*this->stat)(stat)) return true;
	}
	return false;
}
bool Body::operator()(RunStatus& stat)
{
	for(auto x: stats)
	{
		if((*x)(stat)) return true;
	}
	return false;
}

bool Return::operator()(RunStatus& stat)
{
	stat.setStatus(RunStatus::RETURN_REACHED);
	return true;
}

