#ifndef _STATEMENT_H
#define _STATEMENT_H
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

class If:public Statement
{
	private:
	// eval expr to choose path
	std::shared_ptr<Function> expr;
	std::vector<std::shared_ptr<Statement>> paths;
	public:
	virtual void operator()();
	void setExpression(std::shared_ptr<Function> exp);
};

class While:public Statement
{
	public:
	std::shared_ptr<Function> expr;
	std::shared_ptr<Statement> stat;
	public:
	virtual void operator()();
};


class Body: public Statement
{
	public:
	std::vector<std::shared_ptr<Statement>> stats;
	public:
	virtual void operator()();
};

#endif
