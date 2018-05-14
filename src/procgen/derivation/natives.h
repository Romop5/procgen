/**
 * @file 'derivation/natives.h'
 * @brief Contains native functions for derivation module
 * @copyright The MIT license 
 */
#ifndef _NATIVES_H
#define _NATIVES_H
// Native functions for derivation

#include <procgen/derivation/derivation.h>
#include <procgen/interpret/function.h>
#include <procgen/utils/logger.h>

namespace ProcGen {
// int = getCurrentPosition() returns current position in derivation string
class NativeCurrentPosition : public Function {
    std::weak_ptr<Derivation> derivation;

public:
    NativeCurrentPosition(std::weak_ptr<Derivation> de)
    {
        this->derivation = de;
        this->bindOutput(derivation.lock()->tr.lock()->sharedResource("int"));
    }

    bool operator()(RunStatus& rs)
    {
        *(int*)this->getOutput()->getData() = derivation.lock()->getCurrentStringPositionId();
        return false;
    }
};

// int = getCurrentStringId() returns current string id
class NativeCurrentStringId : public Function {
    std::weak_ptr<Derivation> derivation;

public:
    NativeCurrentStringId(std::shared_ptr<Derivation> de)
    {
        this->derivation = de;
        this->bindOutput(derivation.lock()->tr.lock()->sharedResource("int"));
    }

    bool operator()(RunStatus& rs)
    {
        *(int*)this->getOutput()->getData() = derivation.lock()->getCurrentStringId();
        return false;
    }
};

// int = getSymbol(stringID,posID) returns symbol at [stringId,posid]
class NativeGetSymbol : public Function {
    std::weak_ptr<Derivation> derivation;

public:
    NativeGetSymbol(std::shared_ptr<Derivation> de)
    {
        this->derivation = de;
        this->bindOutput(derivation.lock()->tr.lock()->sharedResource("any"));
        _numberOfExpectedBindings = 2;
    }
    bool bindInput(size_t id, std::shared_ptr<Function> fn)
    {
        if (fn->getOutput()->getTypeName() != "int")
            return false;
        return Function::bindInput(id, fn);
    }
    bool operator()(RunStatus& rs)
    {
        if (_doInputs(rs))
            return true;

        assert(_getInput(0) != nullptr);
        assert(_getInput(0)->getOutput()->getData() != nullptr);
        assert(_getInput(1) != nullptr);
        assert(_getInput(1)->getOutput()->getData() != nullptr);

        int stringId = *(int*)_getInput(0)->getOutput()->getData();
        int positionId = *(int*)_getInput(1)->getOutput()->getData();

        this->bindOutput(derivation.lock()->getSymbolAtPosition(stringId, positionId));
        LOG_DEBUG("getSymbol (%d, %d): %s\n", stringId, positionId, this->getOutput()->to_json().dump().c_str());
        return false;
    }
};

// int = getSymbol(stringID,posID) returns symbol at [stringId,posid]
class NativeHasSymbol : public Function {
    std::weak_ptr<Derivation> derivation;

public:
    NativeHasSymbol(std::shared_ptr<Derivation> de)
    {
        this->derivation = de;
        this->bindOutput(derivation.lock()->tr.lock()->sharedResource("bool"));
        _numberOfExpectedBindings = 2;
    }
    bool bindInput(size_t id, std::shared_ptr<Function> fn)
    {
        if (fn->getOutput()->getTypeName() != "int")
            return false;
        return Function::bindInput(id, fn);
    }
    bool operator()(RunStatus& rs)
    {
        if (_doInputs(rs))
            return true;

        assert(_getInput(0) != nullptr);
        assert(_getInput(0)->getOutput()->getData() != nullptr);
        assert(_getInput(1) != nullptr);
        assert(_getInput(1)->getOutput()->getData() != nullptr);

        int stringId = *(int*)_getInput(0)->getOutput()->getData();
        int positionId = *(int*)_getInput(1)->getOutput()->getData();

        *(bool*)this->getOutput()->getData() = derivation.lock()->hasSymbolAtPosition(stringId, positionId);
        LOG_DEBUG("hasSymbol (%d, %d): %d\n", stringId, positionId, *(bool*)this->getOutput()->getData());
        return false;
    }
};

class NativeGetParent : public Function {
    std::weak_ptr<Derivation> derivation;

public:
    NativeGetParent(std::shared_ptr<Derivation> de)
    {
        this->derivation = de;
        this->bindOutput(derivation.lock()->tr.lock()->sharedResource("int"));
        _numberOfExpectedBindings = 2;
    }
    bool bindInput(size_t id, std::shared_ptr<Function> fn)
    {
        if (fn->getOutput()->getTypeName() != "int")
            return false;
        return Function::bindInput(id, fn);
    }

    bool operator()(RunStatus& rs)
    {
        if (_doInputs(rs))
            return true;

        assert(_getInput(0) != nullptr);
        assert(_getInput(0)->getOutput()->getData() != nullptr);
        assert(_getInput(1) != nullptr);
        assert(_getInput(1)->getOutput()->getData() != nullptr);

        int stringId = *(int*)_getInput(0)->getOutput()->getData();
        int positionId = *(int*)_getInput(1)->getOutput()->getData();

        LOG_DEBUG("getParent() stringId: %d position %d\n", stringId, positionId);

        *(int*)this->getOutput()->getData() = (derivation.lock()->getParentId(stringId, positionId));
        return false;
    }
};

// void skipSymbol(void)
class NativeSkipSymbol : public Function {
    std::weak_ptr<Derivation> derivation;

public:
    NativeSkipSymbol(std::shared_ptr<Derivation> de)
    {
        this->derivation = de;
    }

    bool operator()(RunStatus& rs)
    {
        if (_doInputs(rs))
            return true;

        // Skip another symbol
        this->derivation.lock()->skipSymbol();
        LOG_DEBUG("skipSymbol++\n");
        return false;
    }
};

class NativeIteratorLimit : public Function {
    std::shared_ptr<Derivation> derivation;

public:
    NativeIteratorLimit(std::shared_ptr<Derivation> de)
    {
        this->derivation = de;
        _numberOfExpectedBindings = 1;
    }

    bool bindInput(size_t id, std::shared_ptr<Function> fn)
    {
        if (fn->getOutput()->getTypeName() != "int")
            return false;
        return Function::bindInput(id, fn);
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

class NativeAppendSymbol : public Function {
    std::weak_ptr<Derivation> derivation;

public:
    NativeAppendSymbol(std::weak_ptr<Derivation> de)
    {
        _numberOfExpectedBindings = 1;
        this->derivation = de;
    }

    bool operator()(RunStatus& rs)
    {
        if (_getInput(0) != nullptr) {
            (*_getInput(0))(rs);
        }

        std::string dmp = _getInput(0)->getOutput()->to_json().dump();

        LOG_DEBUG("AppendSymbol: '%s\n'", dmp.c_str());

        std::shared_ptr<Resource> src = derivation.lock()->tr.lock()->sharedResource(_getInput(0)->getOutput()->getBaseId());
        if (src->copy(_getInput(0)->getOutput()) == false)
            return false;
        derivation.lock()->appendNextSymbol(src);

        return false;
    }
};
}
#endif
