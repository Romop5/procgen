#ifndef _STATEMENT_H
#define _STATEMENT_H
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include "resource.h"

class RunStatus
{
	public:
	enum status { OK, RETURN_REACHED, RUNTIME_ERROR} _status;
	void setStatus(status _status) {this->_status = _status;}
	status getStatus() {return this->_status;}
};
class Function;
class Statement
{
	public:
	virtual bool operator () (RunStatus&) = 0;
};

class If:public Statement
{
	private:
	// eval expr to choose path
	std::shared_ptr<Function> expr;
	std::vector<std::shared_ptr<Statement>> paths;
	public:
	virtual bool operator()(RunStatus&);
	void setExpression(std::shared_ptr<Function> exp);
};

class While:public Statement
{
	public:
	std::shared_ptr<Function> expr;
	std::shared_ptr<Statement> stat;
	public:
	virtual bool operator()(RunStatus&);
};


class Body: public Statement
{
	public:
	std::vector<std::shared_ptr<Statement>> stats;
	public:
	virtual bool operator()(RunStatus&);
};

#endif
