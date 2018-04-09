#ifndef _APPENDER_H
#define _APPENDER_H
#include <procgen/derivation/derivation.h>
#include <procgen/interpret/interpret.h>

class AppendSymbol : public Function {
    std::weak_ptr<Derivation> derivation;

public:
    AppendSymbol(std::weak_ptr<Derivation> de)
    {
        this->derivation = de;
    }

    bool operator()(RunStatus& rs)
    {
        if (_getInput(0) != nullptr) {
            (*_getInput(0))(rs);
        }

        std::string dmp = _getInput(0)->getOutput()->to_json().dump();

        std::cout << "AppendSymbol: '" << dmp << "'" << std::endl;

        std::shared_ptr<Resource> src = derivation.lock()->tr.lock()->sharedResource(_getInput(0)->getOutput()->getBaseId());
        if (src->copy(_getInput(0)->getOutput()) == false)
            return false;
        std::cout << "Appending id " << src->getBaseId() << "\n";
        derivation.lock()->appendNextSymbol(src);

        return false;
    }
};
#endif
