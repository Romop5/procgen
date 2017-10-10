#include "function.h"
void Function::operator()()
{
	for(auto x: children)
		(*x)();
}

void DebugStatement::operator()()
{
	std::cout << "Debugstat:\n";
	unsigned int id = 0;
	for(auto x: this->inputs)
	{
		std::cout << id++ << "\t" << *(int*) x->value << std::endl; 
	}	
}
void Add::operator()()
{
	for(auto x: children)
		(*x)();
	int out = *(int*) inputs[0]->value +*(int*) inputs[1]->value;
	*((int*)output->value) = out;
}

void Copy::operator()()
{
	for(auto x: children)
		(*x)();
	int out = *(int*) inputs[0]->value;
	*((int*)output->value) = out;

}
void Mul::operator()()
{
	for(auto x: children)
		(*x)();
	int out = (*(int*) inputs[0]->value) * (*(int*) inputs[1]->value);
	*((int*)output->value) = out;

}
void Greater::operator()()
{
	for(auto x: children)
		(*x)();
	int out = (*(int*) inputs[0]->value) > (*(int*) inputs[1]->value);
	*((int*)output->value) = out;

}

