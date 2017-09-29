#include <iostream>
#include <vector>
#include <map>
#include <memory>

class Function;
class Resource
{
	public:
	unsigned int type;
	void* value;	
};

class Statement
{
	public:
	virtual void operator () () = 0;
};

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
	virtual void operator()()
	{
		for(auto x: children)
			(*x)();
	}
};


class If:public Statement
{
	private:
	// eval expr to choose path
	std::shared_ptr<Function> expr;
	std::vector<std::shared_ptr<Statement>> paths;
	public:
	virtual void operator()()
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
	void setExpression(std::shared_ptr<Function> exp)
	{
		this->expr = expr;
	}
};

class While:public Statement
{
	std::shared_ptr<Function> expr;
	std::shared_ptr<Statement> stat;
	public:
	virtual void operator()()
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
};

class Body: public Statement
{
	public:
	std::vector<std::shared_ptr<Statement>> stats;
	public:
	virtual void operator()()
	{
		for(auto x: stats)
			(*x)();
	}
};

class Add: public Function
{
	public:
	virtual void operator()()
	{
		for(auto x: children)
			(*x)();
		int out = *(int*) inputs[0]->value +*(int*) inputs[1]->value;
		*((int*)output->value) = out;

	}
};

class Mul: public Function
{
	public:
	virtual void operator()()
	{
		for(auto x: children)
			(*x)();
		int out = (*(int*) inputs[0]->value) * (*(int*) inputs[1]->value);
		*((int*)output->value) = out;

	}
};

class ResourceFactory
{
	private:
	std::map<std::string,std::shared_ptr<Resource>> resources;
	public:
	std::shared_ptr<Resource> createResource(std::string name)
	{
		resources[name] = std::make_shared<Resource>();
		// TODO: alloc mem according to type
		resources[name]->value = new int();
		return resources[name];
	}

	void dump()
	{
		std::cout << "Dumping resources:\n";
		for(auto x: resources)
			std::cout << x.first << " - " << *(int*) x.second->value << std::endl;
	}

};
