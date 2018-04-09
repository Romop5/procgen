#include "../interpret/interpret.h"
#include <procgen/derivation/derivation.h>

class IteratorLimit : public Function {
    std::shared_ptr<Derivation> derivation;

public:
    IteratorLimit(std::shared_ptr<Derivation> de)
    {
        this->derivation = de;
    }

    bool operator()(RunStatus& rs)
    {
        if (_getInput(0) != nullptr)
            (*_getInput(0))(rs);

        std::shared_ptr<Resource> src = _getInput(0)->getOutput();
        int iterator = *(int*)src->getData();
        derivation->setMaximumIterations(iterator);

        return false;
    }
};
