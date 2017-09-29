#ifndef STATEMENT_H
#define STATEMENT_H
#include <iostream>
#include <memory>
#include "resource.h"
class Statement
{
	public:
	using uint = unsigned int;
	virtual void bindInput(uint id, std::shared_ptr<Resource> res) = 0;
	virtual void bindOutput(uint id, std::shared_ptr<Resource> res) = 0;
	virtual void operator()() = 0;
};
#endif
