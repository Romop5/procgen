/*
 * Procedural generation library
 * xdobia11 - Roman Dobias
 * Purpose: defines standard boxes (functions)
 * Description: a Composite pattern for handling functions
 * 		HandleFunction is terminal Function
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
	private:
	std::map<size_t,std::shared_ptr<Function>> _inputs;
	std::shared_ptr<Resource>	_output;


	protected:		
	bool				_doInputs(RunStatus& stat);
	std::shared_ptr<Function>	_getInput(size_t id) {return this->_inputs[id];}
	public:
	virtual bool bindInput(size_t id, std::shared_ptr<Function> func);
	virtual bool bindOutput(std::shared_ptr<Resource> res);
	std::shared_ptr<Resource>	getOutput() { return this->_output; }
	virtual bool operator()(RunStatus& stat);
};

class HandleFunction: public Function
{
	public:
	virtual bool operator()(RunStatus& stat);
};
class DebugStatement:public Function
{
	public:
	virtual bool operator()(RunStatus& stat);
};

template<typename T>
class PrintValue: public Function
{
	public:
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;
		std::cout << *(T*)(_getInput(0)->getOutput()->value) << std::endl;
		return false;
	}
};



// Define a binary operatorion OPNAME<type> 
#define DEF_BINARY_OP(OPNAME,OPERATOR)\
template<typename T>\
class OPNAME: public Function\
{\
	public:\
	virtual bool operator()(RunStatus& stat)\
	{\
		if(_doInputs(stat)) return true;\
		T out = *(T*) _getInput(0)->getOutput()->value OPERATOR *(T*) _getInput(1)->getOutput()->value;\
		*(T*)(getOutput()->value) = out;\
		return false;\
	}\
};

// Define a unary operatorion OPNAME<type> 
#define DEF_UNARY_OP(OPNAME,OPERATOR)\
template<typename T>\
class OPNAME: public Function\
{\
	public:\
	virtual bool operator()(RunStatus& stat)\
	{\
		if(_doInputs(stat)) return true;\
		T out = *(T*) _getInput(0)->getOutput()->value OPERATOR ;\
		*(T*)(getOutput()->value) = out;\
		return false;\
	}\
};

/*
 * Create classes for standard functions (operators)
 */

DEF_UNARY_OP(Copy,);

DEF_BINARY_OP(Add,+);
DEF_BINARY_OP(Sub,-);
DEF_BINARY_OP(Mul,*);
DEF_BINARY_OP(Div,/);
DEF_BINARY_OP(Greater,>);
DEF_BINARY_OP(Less,<);
DEF_BINARY_OP(Eq,==);
#endif
