/*
 * Procedural generation library
 * xdobia11 - Roman Dobias
 * Purpose: defines standard boxes (functions)
 *
 */
#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H
#include <iostream>
#include <vector>
#include <map>
#include <memory>

#include "statement.h"
class Statement;
class Resource;
class Function: public Statement
{
	// inputs = functions
	// output = resource
	public:
	std::vector<std::shared_ptr<Function>> children;
	std::vector<std::shared_ptr<Resource>> inputs;
	std::shared_ptr<Resource>	output;
	public:
	std::shared_ptr<Resource>	getOutput() { return this->output; }
	virtual void operator()();
};


class DebugStatement:public Function
{
	public:
	virtual void operator()();
};

class Add: public Function
{
	public:
	virtual void operator()();
};

class Copy: public Function
{
	public:
	virtual void operator()();
};

class Mul: public Function
{
	public:
	virtual void operator()();
};

class Greater: public Function
{
	public:
	virtual void operator()();
};
/*
template<typename T>
class tAdd: public Function
{
	public:
	//virtual void operator()();
	virtual void operator()()
	{
		for(auto x: children)
			(*x)();
		T out = *(T*) inputs[0]->value +*(T*) inputs[1]->value;
		*((T*)output->value) = out;
	}


};
*/

// Define a binary operatorion OPNAME<type> 
#define DEF_BINARY_OP(OPNAME,OPERATOR)\
template<typename T>\
class t##OPNAME: public Function\
{\
	public:\
	virtual void operator()()\
	{\
		for(auto x: children)\
			(*x)();\
		T out = *(T*) inputs[0]->value OPERATOR *(T*) inputs[1]->value;\
		*((T*)output->value) = out;\
	}\
};

DEF_BINARY_OP(Add,+);
DEF_BINARY_OP(Mul,*);
DEF_BINARY_OP(Div,/);
DEF_BINARY_OP(Greater,>);
DEF_BINARY_OP(Eg,==);
#endif
