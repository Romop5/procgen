#include "function.h"
bool Function::operator()(RunStatus& stat)
{
	for(auto x: children)
	{
		bool result = (*x)(stat);
		if(result) return true;
	}
	return false;
}

bool DebugStatement::operator()(RunStatus& stat)
{
	std::cout << "Debugstat:\n";
	unsigned int id = 0;
	for(auto x: this->inputs)
	{
		std::cout << id++ << "\t" << *(int*) x->value << std::endl; 
	}	
	return false;
}
bool Add::operator()(RunStatus& stat)
{
	for(auto x: children)
	{
		bool result = (*x)(stat);
		if(result) return true;
	}
	int out = *(int*) inputs[0]->value +*(int*) inputs[1]->value;
	*((int*)output->value) = out;
	return false;
}

bool Copy::operator()(RunStatus& stat)
{
	for(auto x: children)
	{
		bool result = (*x)(stat);
		if(result) return true;
	}
	int out = *(int*) inputs[0]->value;
	*((int*)output->value) = out;
	return false;

}
bool Mul::operator()(RunStatus& stat)
{
	for(auto x: children)
	{
		bool result = (*x)(stat);
		if(result) return true;
	}
	int out = (*(int*) inputs[0]->value) * (*(int*) inputs[1]->value);
	*((int*)output->value) = out;
	return false;

}
bool Greater::operator()(RunStatus& stat)
{
	for(auto x: children)
	{
		bool result = (*x)(stat);
		if(result) return true;
	}
	int out = (*(int*) inputs[0]->value) > (*(int*) inputs[1]->value);
	*((int*)output->value) = out;
	return false;

}

/*
template<typename T>
bool tAdd<T>::operator()(RunStatus& stat)
{
	for(auto x: children)
	{
		bool result = (*x)(stat);
		if(result) return true;
	}
	T out = *(T*) inputs[0]->value +*(T*) inputs[1]->value;
	*((T*)output->value) = out;
}

*/
