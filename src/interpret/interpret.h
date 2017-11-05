#include "typereg.h"
#include "resource.h"
#include "functionreg.h"
#include "variablereg.h"
#include <stack>

using nodeId = size_t;

class Interpret {
	private:
	std::map<size_t, std::shared_ptr<Statement>> nodes;

	std::stack<nodeId> bodies;
	public:

	std::shared_ptr<FunctionReg> fr;
	std::shared_ptr<TypeRegister> tr;
	std::shared_ptr<VariableReg> vr;
	Interpret(std::shared_ptr<TypeRegister> tr, std::shared_ptr<FunctionReg> fr)
	{
		this->fr = fr;
		this->tr = tr;
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
		*(BASE*) constVar->value = value;
		return this->addNode(fr->getHandler(constVar));
	}

	nodeId createOperation(const std::string operation,nodeId right, nodeId left)
	{
		const std::string typeName = this->getFunction(right)->getOutput()->getName();
		auto op = this->fr->getFunc(operation+":"+typeName);
		auto res = this->tr->sharedResource(typeName);
		op->bindOutput(res);
		op->bindInput(0,this->getFunction(left));
		op->bindInput(1,this->getFunction(right));
		return addNode(op);
	}

	nodeId createAssignment(const std::string varname, nodeId exprTree)
	{
		auto var = this->vr->getVar(varname);
		const std::string typeName = this->getFunction(exprTree)->getOutput()->getName();
		auto op = this->fr->getFunc("Copy"+typeName);
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
};



