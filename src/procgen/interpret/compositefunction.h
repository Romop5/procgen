#ifndef COMPOSITE_H
#define COMPOSITE_H
#include <vector>
#include <memory>
#include <string>
#include <procgen/interpret/resource.h>
#include <procgen/interpret/statement.h>
#include <procgen/interpret/function.h>
#include <procgen/utils/logger.h>

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
	std::weak_ptr<CompositeFunction> cf;
    std::string name;
	public:
	FunctionCall(std::shared_ptr<CompositeFunction> cf,std::string name)
	{
		this->cf = cf;
		this->bindOutput(this->cf.lock()->output->allocateClone());
        this->name = name;
	}
	bool operator()(RunStatus& stat)
	{
		/* Prepare arguments */
		if(this->_doInputs(stat))
			return true;
		// TODO: checkout type equivalence 
		// bind inputs to interface

        LOG_DEBUG("Invoking function: %s\n",this->name.c_str());
		for(int i=0; i < cf.lock()->inputs.size();i++)
		{
            assert(this->_getInput(i) != nullptr);
            assert(this->_getInput(i)->getOutput() != nullptr);

            std::string dmp = this->_getInput(i)->getOutput()->to_json().dump();
            LOG_DEBUG("Argument[in] %d %s\n",i,dmp.c_str());
			cf.lock()->inputs[i]->copy(this->_getInput(i)->getOutput());
            dmp = cf.lock()->inputs[i]->to_json().dump();
            LOG_DEBUG("Argument[inside] %d %s\n",i,dmp.c_str());
		}
		// process function
		if(this->getOutput() != nullptr)
        {
            std::string dmp = this->getOutput()->to_json().dump();
            LOG_DEBUG("Output[before] '%s'\n",dmp.c_str());
        }

		bool result = (*cf.lock()->core)(stat);

        if(this->getOutput() != nullptr)
        {
            std::string dmp = this->getOutput()->to_json().dump();
            LOG_DEBUG("Output[after] '%s'\n",dmp.c_str());
        }

		// copy result
		if(this->getOutput() != nullptr)
			this->getOutput()->copy(cf.lock()->output);

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


    bool bindInput(size_t id, std::shared_ptr<Function> func) override
    {
        if(func->getOutput() == nullptr)
            return false;
        if(!this->cf.lock()->inputs[id]->hasSameType(func->getOutput()))
            return false;
        return Function::bindInput(id, func);
    }
    bool bindOutput(std::shared_ptr<Resource> res) override
    {
        if(!this->cf.lock()->output->hasSameType(res))
            return false;
        return Function::bindOutput(res);
    }



};

#endif
