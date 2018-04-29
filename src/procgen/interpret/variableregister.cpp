#include <iostream>
#include <procgen/interpret/variableregister.h>
#include <procgen/utils/logger.h>

namespace ProcGen {

bool VariableReg::addVar(std::string name, std::shared_ptr<Resource> src)
{
    if (this->vars.find(name) != this->vars.end())
        return false;
    this->vars[name] = src;
    return true;
}
std::shared_ptr<Resource> VariableReg::getVar(std::string name)
{
    auto it = this->vars.find(name);
    if (it != this->vars.end()) {
        return this->vars[name];
    }
    return nullptr;
}

// Print out all info
void VariableReg::_debug()
{
    for (auto& x : this->vars) {
        LOG_DEBUG("VR: %s\n", x.first.c_str());
    }
}

void VariableReg::clear()
{
    this->vars.clear();
}
}
