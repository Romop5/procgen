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


