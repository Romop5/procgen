#ifndef _INTERPRET_H
#define _INTERPRET_H
#include "typereg.h"
#include "resource.h"
#include "functionreg.h"
#include "variablereg.h"
#include "std.h"
#include <stack>

#include <error.h>

using nodeId = size_t;

class Interpret {
	private:
	std::map<size_t, std::shared_ptr<Statement>> nodes;

	std::stack<nodeId> bodies;

	std::vector<std::shared_ptr<Resource>> params;
	std::stack<std::string> lastString;
	public:

	std::shared_ptr<FunctionReg> fr;
	std::shared_ptr<TypeRegister> tr;
	std::shared_ptr<VariableReg> vr;
	Interpret(std::shared_ptr<TypeRegister> tr, std::shared_ptr<FunctionReg> fr, std::shared_ptr<VariableReg> vr)
	{
		this->fr = fr;
		this->tr = tr;
		this->vr = vr;
	}
	size_t addNode(std::shared_ptr<Statement> st) 
	{
		static size_t i = 0;
		this->nodes[i] = st;
		return i++;
	}
	std::shared_ptr<Statement> getNode(size_t id) { return this->nodes[id];}
	std::shared_ptr<Function> getFunction(size_t id) { return std::static_pointer_cast<Function>(this->nodes[id]);}

	/*
	 * Following code is to parse language 
	 */
	template <class BASE>
	nodeId createResource(BASE value)
	{
		auto constVar = this->tr->sharedResource(keyword<BASE>());
		*(BASE*) constVar->getData() = value;
		return this->addNode(fr->getHandler(constVar));
	}

	nodeId createOperation(const std::string operation,nodeId right, nodeId left)
	{
		const std::string typeName = this->getFunction(right)->getOutput()->getName();
		auto op = this->fr->getFunc(operation+":"+typeName);
		if(op == nullptr)
			error(1,0,"FUCK, type not found for op");
		auto res = this->tr->sharedResource(typeName);
		if(res == nullptr)
			error(1,0,"FUCK, type not found");
		op->bindOutput(res);
		op->bindInput(0,this->getFunction(left));
		op->bindInput(1,this->getFunction(right));
		return addNode(op);
	}

	nodeId createAssignment(const std::string varname, nodeId exprTree)
	{
		auto var = this->vr->getVar(varname);
		if(var == nullptr)
			error(1,0,"Missing variable %s", varname.c_str());
		const std::string typeName = this->getFunction(exprTree)->getOutput()->getName();
		auto op = this->fr->getFunc("Copy:"+typeName);
		op->bindOutput(var);
		op->bindInput(0, this->getFunction(exprTree));
		return addNode(op);
	}

	nodeId createNOOP()
	{
		class NOP: public Function
		{
		};
		return addNode(std::make_shared<NOP>());
	}

	/* Statements and their parsing */

	void pushBody(nodeId body)
	{
		this->bodies.push(body);
	}
	nodeId popBody()
	{
		auto top = this->topBody();
		this->bodies.pop();
		return top;
	}
	nodeId topBody()
	{
		return this->bodies.top();
	}

	void paramPush(std::shared_ptr<Resource> param)
	{
		this->params.push_back(param);
	}

	void paramClear()
	{
		this->params.clear();
	}

	std::vector<std::shared_ptr<Resource>>& paramGet()
	{
		return this->params;
	}	

	void addLastString(const std::string & str)
	{
		this->lastString.push(str);	
	}
	const std::string & getLastString()
	{
		return this->lastString.top();
	}
	void popLastString()
	{
		this->lastString.pop();
	}



	std::vector<TypeId> structDef;
};



#endif
