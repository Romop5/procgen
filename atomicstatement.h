#include <iostream>
#include "statement.h"
class AtomicStatement: public Statement
{
	public:
	virtual void operator()()
	{
		std::cout << "This is atomic,bitch" << std::endl;
	}
};
