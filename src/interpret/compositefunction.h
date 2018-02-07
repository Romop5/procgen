#ifndef COMPOSITE_H
#define COMPOSITE_H
#include <vector>
#include <memory>
#include "resource.h"
#include "statement.h"
#include "function.h"

/*
1. Create an expression tree with vars
2. Define a subset of public variables and output
3. Register tree, public vars and output

4. Create function call box for this composite function.
	On call, call box will set new content of public variables, process the called function
	and copy the result value back.

5. Enjoy it
*/
// Will handle tree and interfaces
struct CompositeFunction
{
	std::vector<std::shared_ptr<Resource>> inputs; 
	std::shared_ptr<Resource> output;
	std::shared_ptr<Statement> core;
	public:
		CompositeFunction(
			std::shared_ptr<Statement> core,
			std::vector<std::shared_ptr<Resource>> inputs, 
			std::shared_ptr<Resource> output)
		{

			this->core = core;
			this->inputs = inputs;
			this->output = output;
		}

};


class FunctionCall : public Function
{
	std::shared_ptr<CompositeFunction> cf;
	public:
	FunctionCall(std::shared_ptr<CompositeFunction> cf)
	{
		this->cf = cf;
		this->bindOutput(this->cf->output);
	}
	bool operator()(RunStatus& stat)
	{
		/* Prepare arguments */
		if(this->_doInputs(stat))
			return true;
		// TODO: checkout type equivalence 
		// bind inputs to interface
		for(int i=0; i < cf->inputs.size();i++)
		{

            std::string dmp = this->_getInput(i)->getOutput()->to_json().dump();
            std::cout << "Argument[in] " << i << "'" << dmp  << "'" << std::endl;
			cf->inputs[i]->copy(this->_getInput(i)->getOutput());
            dmp = cf->inputs[i]->to_json().dump();
            std::cout << "Argument[inside] " << i << "'" << dmp  << "'" << std::endl;
		}
		// process function
		if(this->getOutput() != nullptr)
        {
            std::string dmp = this->getOutput()->to_json().dump();
            std::cout << "Output[before] '" << dmp << "'" << std::endl;
        }

		bool result = (*cf->core)(stat);

        if(this->getOutput() != nullptr)
        {
            std::string dmp = this->getOutput()->to_json().dump();
            std::cout << "Output[after] '" << dmp << "'" << std::endl;
        }

		// copy result
		//if(this->getOutput() != nullptr)
		//	this->getOutput()->copy(cf->output);

		// TODO: determine if positive result is caused by return or 
		// by runtime error
		// if it was return, then reset the status
		if(result)
		{
			if(stat.getStatus() != RunStatus::RETURN_REACHED)
				return true;
			stat.setStatus(RunStatus::OK);
		}

		return false;
	}
};

#endif
