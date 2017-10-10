#include "statement.h"
#include "function.h"
void If::operator()()
{
	// get result of expression
	(*expr)();

	//expr.getOutput().value
	bool first = true;
	// evaluate
	if(first)
		(*paths[0])();
	else
		(*paths[1])();

}
void If::setExpression(std::shared_ptr<Function> exp)
{
	this->expr = expr;
}

void While::operator()()
{
	while(true)
	{
		(*this->expr)();
		bool isTrue = *(bool*) expr->getOutput()->value;
		if(!isTrue)
			return;
		// eval statement
		(*stat)();
	}
}
void Body::operator()()
{
	for(auto x: stats)
		(*x)();
}

