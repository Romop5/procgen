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
	std::map<size_t, std::shared_ptr<Statement>> paths;
	public:
	virtual bool operator()(RunStatus&);
	void setExpression(std::shared_ptr<Function> exp);
	void setPath(size_t id, std::shared_ptr<Statement> path);
};

class While:public Statement
{
	private:
	std::shared_ptr<Function> _expression;
	std::shared_ptr<Statement> _statement;
	public:
    void bindCondition(std::shared_ptr<Function> expression) { this->_expression = expression; }
    void bindStatement(std::shared_ptr<Statement> statement) { this->_statement = statement; }

	virtual bool operator()(RunStatus&);
};


class Body: public Statement
{
	public:
	std::vector<std::shared_ptr<Statement>> stats;
	public:
	void appendStatement(std::shared_ptr<Statement> stat) {this->stats.push_back(stat);}
	virtual bool operator()(RunStatus&);
};

class Return: public Statement
{
	public:
	std::shared_ptr<Statement> input;
	std::shared_ptr<Statement> output;
	public:
	virtual bool operator()(RunStatus&);
};


#endif
