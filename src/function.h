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
	virtual bool operator()(RunStatus& stat);
};


class DebugStatement:public Function
{
	public:
	virtual bool operator()(RunStatus& stat);
};

class Add: public Function
{
	public:
	virtual bool operator()(RunStatus& stat);
};

class Copy: public Function
{
	public:
	virtual bool operator()(RunStatus& stat);
};

class Mul: public Function
{
	public:
	virtual bool operator()(RunStatus& stat);
};

class Greater: public Function
{
	public:
	virtual bool operator()(RunStatus& stat);
};
/*
template<typename T>
class tAdd: public Function
{
	public:
	//virtual bool operator()(RunStatus& stat);
	virtual bool operator()(RunStatus& stat)
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
	virtual bool operator()(RunStatus& stat)\
	{\
		for(auto x: children)\
			if((*x)(stat)) return true;\
		T out = *(T*) inputs[0]->value OPERATOR *(T*) inputs[1]->value;\
		*((T*)output->value) = out;\
		return false;\
	}\
};

// Define a unary operatorion OPNAME<type> 
#define DEF_UNARY_OP(OPNAME,OPERATOR)\
template<typename T>\
class t##OPNAME: public Function\
{\
	public:\
	virtual bool operator()(RunStatus& stat)\
	{\
		for(auto x: children)\
			if((*x)(stat)) return true;\
		T out = *(T*) inputs[0]->value OPERATOR ;\
		*((T*)output->value) = out;\
		return false;\
	}\
};


// Copy input to output
DEF_UNARY_OP(Copy,);

DEF_BINARY_OP(Add,+);
DEF_BINARY_OP(Sub,-);
DEF_BINARY_OP(Mul,*);
DEF_BINARY_OP(Div,/);
DEF_BINARY_OP(Greater,>);
DEF_BINARY_OP(Eq,==);
#endif
