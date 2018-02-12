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
#include <exception>
#include <climits>

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

class PrintJson: public Function
{
	public:
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;
		std::cout << _getInput(0)->getOutput()->to_json().dump() << std::endl;
		return false;
	}
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




// Define a binary logic operatorion OPNAME<type> 
#define DEF_BINARY_LOGIC_OP(OPNAME,OPERATOR)\
template<typename T>\
class OPNAME: public Function\
{\
	public:\
	virtual bool operator()(RunStatus& stat)\
	{\
		if(_doInputs(stat)) return true;\
		bool out = *(T*) _getInput(0)->getOutput()->getData() OPERATOR *(T*) _getInput(1)->getOutput()->getData();\
		*(bool*)(getOutput()->getData()) = out;\
		return false;\
	}\
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

// Define a unary operatorion OPNAME<type> 
#define DEF_PREFIX_UNARY_OP(OPNAME,OPERATOR)\
template<typename T>\
class OPNAME: public Function\
{\
	public:\
	virtual bool operator()(RunStatus& stat)\
	{\
		if(_doInputs(stat)) return true;\
		T out = OPERATOR *(T*) _getInput(0)->getOutput()->getData();\
		*(T*)(getOutput()->getData()) = out;\
		std::cout << #OPNAME << "\n";\
		return false;\
	}\
};

// Define a unary operatorion OPNAME<type> 
#define DEF_PREFIX_BOOL_UNARY_OP(OPNAME,OPERATOR)\
template<typename T>\
class OPNAME: public Function\
{\
	public:\
	virtual bool operator()(RunStatus& stat)\
	{\
		if(_doInputs(stat)) return true;\
		bool out = OPERATOR *(T*) _getInput(0)->getOutput()->getData();\
		*(bool*)(getOutput()->getData()) = out;\
		std::cout << #OPNAME << "\n";\
		return false;\
	}\
};




// input(0) = input(1)
class GenericCopy: public Function
{
	public:
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;
		_getInput(0)->getOutput()->copy(_getInput(1)->getOutput());
		this->bindOutput(_getInput(0)->getOutput());
		return false;
	}
};


// Run-time verification of type
class Convert: public Function 
{
	private:
	TypeId type;
	public:
	Convert(TypeId _type):type(_type) {};
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;
		if(_getInput(0)->getOutput()->getBaseId() != type)
		{
			// error = types doesn't match
			throw std::runtime_error("Convert<>: types don't match");
		}
		this->bindOutput(_getInput(0)->getOutput());
		return false;
	}

};

// Returns run-time TypeId for given resource
// TypeId is int
class GetTypeId: public Function 
{
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;
		// Write TypeId to output resource
		*(int*) this->getOutput()->getData() = (_getInput(0)->getOutput()->getBaseId());
		return false;
	}

};

// Random in range <a,b)
class GenerateUniform: public Function
{
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;

		// a = 0
		// b = 1

		float a = *(float*) _getInput(0)->getOutput()->getData();
		float b = *(float*) _getInput(1)->getOutput()->getData();

		float x = random() / (float(UINT_MAX)+1.0);
		x = x*(b-a)+a;
		*(float*) this->getOutput()->getData() = x;
	}
};

// Random in range <0,1)
class GenerateRandom: public Function
{
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;

		*(float*) this->getOutput()->getData() = random() / (float(UINT_MAX)+1.0);
	}
};




/*
 * Create classes for standard functions (operators)
 */

DEF_UNARY_OP(Copy,);

DEF_PREFIX_BOOL_UNARY_OP(Negation, !);
DEF_PREFIX_UNARY_OP(UnaryMinus, -);
DEF_PREFIX_UNARY_OP(UnaryPlus, +);

DEF_BINARY_OP(Add,+);
DEF_BINARY_OP(Sub,-);
DEF_BINARY_OP(Mul,*);
DEF_BINARY_OP(Div,/);
DEF_BINARY_LOGIC_OP(Greater,>);
DEF_BINARY_LOGIC_OP(Less,<);
DEF_BINARY_LOGIC_OP(Eq,==);
DEF_BINARY_LOGIC_OP(NotEq,!=);
#endif
