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
		std::cout << *(T*)(_getInput(0)->getOutput()->getData()) << std::endl;
		return false;
	}
};

class CollectionAppend:public Function
{
	public:
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;

		std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput())->append(_getInput(1)->getOutput());

		return false;
	}
};

class CollectionIndex:public Function
{
	public:
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;

		// TODO check if 0 is collection and 1 is size_t
		auto res = std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput());
		this->bindOutput(res->at(*(int*)_getInput(1)->getOutput()->getData()));
		return false;
	}
};

class CollectionLength:public Function
{
	public:
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;

		// TODO check if 0 is collection and 1 is size_t
		auto res = std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput());
		*(int*) this->getOutput()->getData() = res->length();
		return false;
	}
};

class CollectionRemove:public Function
{
	public:
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;

		// TODO check if 0 is collection and 1 is size_t
		auto res = std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput());
		res->remove(*(int*)_getInput(1)->getOutput()->getData());
		return false;
	}
};

/*
 * Input 0 = struct
 * Input 1 = index
 * Output = part 
 */
class CompositeGet:public Function
{
	public:
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;

		// TODO check if 0 is collection and 1 is size_t
		auto res = std::dynamic_pointer_cast<CompositeResource>(_getInput(0)->getOutput());
		size_t index = *(int*)_getInput(1)->getOutput()->getData();
		this->bindOutput(res->getComponent(index));
		return false;
	}
};

/*
 * Input 0 = struct
 * Input 1 = index
 * Input 2 = part 
 */
class CompositeSet:public Function
{
	public:
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;

		// TODO check if 0 is collection and 1 is size_t
		auto res = std::dynamic_pointer_cast<CompositeResource>(_getInput(0)->getOutput());
		size_t index = *(int*)_getInput(1)->getOutput()->getData();
		res->getComponent(index)->copy(_getInput(2)->getOutput());
		return false;
	}
};


// TODO: create macro type2type
class FloatToBool:public Function
{
	public:
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;

		bool out =(bool) *(float*) _getInput(0)->getOutput()->getData();
		*(bool*)(getOutput()->getData()) = out;
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
		T out = *(T*) _getInput(0)->getOutput()->getData() OPERATOR *(T*) _getInput(1)->getOutput()->getData();\
		*(T*)(getOutput()->getData()) = out;\
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
		T out = *(T*) _getInput(0)->getOutput()->getData() OPERATOR ;\
		*(T*)(getOutput()->getData()) = out;\
		std::cout << "Copying\n";\
		return false;\
	}\
};

class GenericCopy: public Function\
{
	public:
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;
		_getInput(0)->getOutput()->copy(_getInput(1)->getOutput());
        this->bindOutput(_getInput(0)->getOutput());
		return false;\
	}
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
