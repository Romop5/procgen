#include <procgen/interpret/compositefunction.h>
namespace ProcGen {
CompositeFunction::CompositeFunction(
    std::shared_ptr<Statement> core,
    std::vector<std::shared_ptr<Resource>> inputs,
    std::shared_ptr<Resource> output)
{

    this->core = core;
    this->inputs = inputs;
    this->output = output;
}

FunctionCall::FunctionCall(std::shared_ptr<CompositeFunction> cf, std::string name)
{
    this->cf = cf;
    this->bindOutput(this->cf.lock()->output->allocateClone());
    this->name = name;

    // set the number of expected bindings
    _numberOfExpectedBindings = cf->inputs.size();
}

bool FunctionCall::operator()(RunStatus& stat)
{
    /* Prepare arguments */
    if (this->_doInputs(stat))
        return true;
    // TODO: checkout type equivalence
    // bind inputs to interface

    LOG_DEBUG("Invoking function: %s\n", this->name.c_str());
    for (size_t i = 0; i < cf.lock()->inputs.size(); i++) {
        assert(this->_getInput(i) != nullptr);
        assert(this->_getInput(i)->getOutput() != nullptr);

        std::string dmp = this->_getInput(i)->getOutput()->to_json().dump();
        LOG_DEBUG("Argument[in] %d %s\n", i, dmp.c_str());
        cf.lock()->inputs[i]->copy(this->_getInput(i)->getOutput());
        dmp = cf.lock()->inputs[i]->to_json().dump();
        LOG_DEBUG("Argument[inside] %d %s\n", i, dmp.c_str());
    }
    // process function
    if (this->getOutput() != nullptr) {
        std::string dmp = this->getOutput()->to_json().dump();
        LOG_DEBUG("Output[before] '%s'\n", dmp.c_str());
    }

    bool result = (*cf.lock()->core)(stat);

    if (this->getOutput() != nullptr) {
        std::string dmp = this->getOutput()->to_json().dump();
        LOG_DEBUG("Output[after] '%s'\n", dmp.c_str());
    }

    // copy result
    if (this->getOutput() != nullptr)
        this->getOutput()->copy(cf.lock()->output);

    // TODO: determine if positive result is caused by return or
    // by runtime error
    // if it was return, then reset the status
    if (result) {
        if (stat.getStatus() != RunStatus::RETURN_REACHED)
            return true;
        stat.setStatus(RunStatus::OK);
    }

    return false;
}

bool FunctionCall::bindInput(size_t id, std::shared_ptr<Function> func)
{
    if (func->getOutput() == nullptr)
        return false;
    if (!this->cf.lock()->inputs[id]->hasSameType(func->getOutput()))
        return false;
    return Function::bindInput(id, func);
}
bool FunctionCall::bindOutput(std::shared_ptr<Resource> res)
{
    if (!this->cf.lock()->output->hasSameType(res))
        return false;
    return Function::bindOutput(res);
}
}
