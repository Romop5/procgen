/*
 * Procedural generation library
 * xdobia11 - Roman Dobias
 * Purpose: defines standard boxes (functions)
 * Description: a Composite pattern for handling functions
 * 		HandleFunction is terminal Function
 */
#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H
#include <climits>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <procgen/interpret/statement.h>
#include <procgen/interpret/types.h>
#include <procgen/utils/logger.h>
class Statement;
class Resource;

/**
* @class Function
* @brief Specialized statement which has inputs and output
*
* Classes based on Function can be tied together using inputs and can pass
* value through output and input ports.
*/
class Function : public Statement {
    // inputs = functions
    // output = resource
private:
    std::map<size_t, std::shared_ptr<Function>> _inputs;
    std::shared_ptr<Resource> _output;

protected:
    bool _doInputs(RunStatus& stat);
    std::shared_ptr<Function> _getInput(size_t id) { return this->_inputs[id]; }

public:
    /**
* @brief Bind func expression as input at slot id
* @param id
* @param func
*/
    virtual bool bindInput(size_t id, std::shared_ptr<Function> func);
    /**
* @brief Set resource res as function storage place
* @param res
*/
    virtual bool bindOutput(std::shared_ptr<Resource> res);

    std::shared_ptr<Resource> getOutput() { return this->_output; }

    size_t getCountOfInputs() const { return this->_inputs.size(); }
    virtual bool operator()(RunStatus& stat);
};

class HandleFunction : public Function {
public:
    virtual bool operator()(RunStatus& stat);
};
class DebugStatement : public Function {
public:
    virtual bool operator()(RunStatus& stat);
};

/**
* @class PrintJson
* @brief Function block for debugging purposes
*/
class PrintJson : public Function {
public:
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;
        LOG_INFO("PrintJson %s\n", _getInput(0)->getOutput()->to_json().dump().c_str());
        return false;
    }
};

template <typename T>
class PrintValue : public Function {
public:
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;
        LOG_INFO("Operation: %g\n", *(T*)(_getInput(0)->getOutput()->getData()));
        return false;
    }
};

/**
* @class CollectionAppend
* @brief Appends new element into collection
*
* Input 0 - collection
* Input 1 - element 
*/
class CollectionAppend : public Function {
public:
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;

        std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput())->append(_getInput(1)->getOutput());

        return false;
    }
};

/**
* @class CollectionIndex
* @brief Outputs collection element, specified by index
*
* Input 0 - collection
* Input 1 - index 
* Output  - element resource
*/
class CollectionIndex : public Function {
public:
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;

        // TODO check if 0 is collection and 1 is size_t
        auto res = std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput());
        this->bindOutput(res->at(*(int*)_getInput(1)->getOutput()->getData()));
        return false;
    }
};

/**
* @class CollectionLength
* @brief Returns the length of collection
*
* Input 0 - collection
* Output  - length resource (size_t)
*/
class CollectionLength : public Function {
public:
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;

        // TODO check if 0 is collection and 1 is size_t
        auto res = std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput());
        *(int*)this->getOutput()->getData() = res->length();
        return false;
    }
};

/**
* @class CollectionRemove
* @brief Remove specified element
*
* Input 0 - collection
* Input 1 - position 
*/
class CollectionRemove : public Function {
public:
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;

        // TODO check if 0 is collection and 1 is size_t
        auto res = std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput());
        res->remove(*(int*)_getInput(1)->getOutput()->getData());
        return false;
    }
};

/**
* @class CollectionClear
* @brief Remove all items
*
* Input 0 - collection
*/
class CollectionClear : public Function {
public:
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;

        auto res = std::dynamic_pointer_cast<CollectionResource>(_getInput(0)->getOutput());
        res->clear();
        return false;
    }
};

/**
* @class CompositeGet
* @brief Get member of structure
*
* Input 0 - collection
* Input 1 - index 
* Output  - element resource
*/
class CompositeGet : public Function {
public:
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;

        // TODO check if 0 is collection and 1 is size_t
        auto res = std::dynamic_pointer_cast<CompositeResource>(_getInput(0)->getOutput());
        size_t index = *(int*)_getInput(1)->getOutput()->getData();
        this->bindOutput(res->getComponent(index));
        return false;
    }
};

/**
* @class CompositeSet
* @brief Set member of structure
*
* Input 0 - collection
* Input 1 - index 
* Input 2 - element
*/

class CompositeSet : public Function {
public:
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;

        // TODO check if 0 is collection and 1 is size_t
        auto res = std::dynamic_pointer_cast<CompositeResource>(_getInput(0)->getOutput());
        size_t index = *(int*)_getInput(1)->getOutput()->getData();
        res->getComponent(index)->copy(_getInput(2)->getOutput());
        return false;
    }
};

// TODO: create macro type2type
class FloatToBool : public Function {
public:
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;

        bool out = (bool)*(float*)_getInput(0)->getOutput()->getData();
        *(bool*)(getOutput()->getData()) = out;
        return false;
    }
};

// Define a binary logic operatorion OPNAME<type>
#define DEF_BINARY_LOGIC_OP(OPNAME, OPERATOR)                                                                         \
    template <typename T>                                                                                             \
    class OPNAME : public Function {                                                                                  \
    public:                                                                                                           \
        virtual bool operator()(RunStatus& stat)                                                                      \
        {                                                                                                             \
            if (_doInputs(stat))                                                                                      \
                return true;                                                                                          \
            bool out = *(T*)_getInput(0)->getOutput()->getData() OPERATOR * (T*)_getInput(1)->getOutput()->getData(); \
            *(bool*)(getOutput()->getData()) = out;                                                                   \
            LOG_DEBUG(#OPNAME " %s = %s " #OPERATOR " %s\n", std::to_string(out).c_str(),                             \
                std::to_string(*(T*)_getInput(0)->getOutput()->getData()).c_str(),                                    \
                std::to_string(*(T*)_getInput(1)->getOutput()->getData()).c_str());                                   \
            return false;                                                                                             \
        }                                                                                                             \
    };

// Define a binary operatorion OPNAME<type>
#define DEF_BINARY_OP(OPNAME, OPERATOR)                                                                            \
    template <typename T>                                                                                          \
    class OPNAME : public Function {                                                                               \
    public:                                                                                                        \
        virtual bool operator()(RunStatus& stat)                                                                   \
        {                                                                                                          \
            if (_doInputs(stat))                                                                                   \
                return true;                                                                                       \
            T out = *(T*)_getInput(0)->getOutput()->getData() OPERATOR * (T*)_getInput(1)->getOutput()->getData(); \
            LOG_DEBUG(#OPNAME ": %s = %s " #OPERATOR " %s\n", std::to_string(out).c_str(),                         \
                std::to_string(*(T*)_getInput(0)->getOutput()->getData()).c_str(),                                 \
                std::to_string(*(T*)_getInput(1)->getOutput()->getData()).c_str());                                \
            *(T*)(getOutput()->getData()) = out;                                                                   \
            return false;                                                                                          \
        }                                                                                                          \
    };

// Define a unary operatorion OPNAME<type>
#define DEF_UNARY_OP(OPNAME, OPERATOR)                                                 \
    template <typename T>                                                              \
    class OPNAME : public Function {                                                   \
    public:                                                                            \
        virtual bool operator()(RunStatus& stat)                                       \
        {                                                                              \
            if (_doInputs(stat))                                                       \
                return true;                                                           \
            T out = *(T*)_getInput(0)->getOutput()->getData() OPERATOR;                \
            LOG_DEBUG(#OPNAME " %s = %s " #OPERATOR "\n", std::to_string(out).c_str(), \
                std::to_string(*(T*)_getInput(0)->getOutput()->getData()).c_str());    \
            *(T*)(getOutput()->getData()) = out;                                       \
            return false;                                                              \
        }                                                                              \
    };

// Define a unary operatorion OPNAME<type>
#define DEF_PREFIX_UNARY_OP(OPNAME, OPERATOR)                                           \
    template <typename T>                                                               \
    class OPNAME : public Function {                                                    \
    public:                                                                             \
        virtual bool operator()(RunStatus& stat)                                        \
        {                                                                               \
            if (_doInputs(stat))                                                        \
                return true;                                                            \
            T out = OPERATOR * (T*)_getInput(0)->getOutput()->getData();                \
            LOG_DEBUG(#OPNAME " %s = " #OPERATOR " %s \n", std::to_string(out).c_str(), \
                std::to_string(*(T*)_getInput(0)->getOutput()->getData()).c_str());     \
            *(T*)(getOutput()->getData()) = out;                                        \
            return false;                                                               \
        }                                                                               \
    };

// Define a unary operatorion OPNAME<type>
#define DEF_PREFIX_BOOL_UNARY_OP(OPNAME, OPERATOR)                                       \
    template <typename T>                                                                \
    class OPNAME : public Function {                                                     \
    public:                                                                              \
        virtual bool operator()(RunStatus& stat)                                         \
        {                                                                                \
            if (_doInputs(stat))                                                         \
                return true;                                                             \
            bool out = OPERATOR * (T*)_getInput(0)->getOutput()->getData();              \
            LOG_DEBUG(#OPNAME " %s =  " #OPERATOR " %s \n", std::to_string(out).c_str(), \
                std::to_string(*(T*)_getInput(0)->getOutput()->getData()).c_str());      \
            *(bool*)(getOutput()->getData()) = out;                                      \
            return false;                                                                \
        }                                                                                \
    };

// Define a unary function operatorion OPNAME<type>
#define DEF_PREFIX_UNARY_FUNCTION(OPNAME, OPERATOR)                                     \
    template <typename T>                                                               \
    class OPNAME : public Function {                                                    \
    public:                                                                             \
        virtual bool operator()(RunStatus& stat)                                        \
        {                                                                               \
            if (_doInputs(stat))                                                        \
                return true;                                                            \
            T out = OPERATOR(*(T*)_getInput(0)->getOutput()->getData());                \
            LOG_DEBUG(#OPNAME " %s = " #OPERATOR " %s \n", std::to_string(out).c_str(), \
                std::to_string(*(T*)_getInput(0)->getOutput()->getData()).c_str());     \
            *(T*)(getOutput()->getData()) = out;                                        \
            return false;                                                               \
        }                                                                               \
    };

// input(0) = input(1)
class GenericCopy : public Function {
public:
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;
        _getInput(0)->getOutput()->copy(_getInput(1)->getOutput());
        this->bindOutput(_getInput(0)->getOutput());
        return false;
    }
};

// Run-time verification of type
class Convert : public Function {
private:
    TypeId type;

public:
    Convert(TypeId _type)
        : type(_type){};
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;
        if (_getInput(0)->getOutput()->getBaseId() != type) {
            // error = types doesn't match
            throw std::runtime_error("Convert<>: types don't match");
        }
        this->bindOutput(_getInput(0)->getOutput());
        return false;
    }
};

// Returns run-time TypeId for given resource
// TypeId is int
class GetTypeId : public Function {
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;
        // Write TypeId to output resource
        *(int*)this->getOutput()->getData() = (_getInput(0)->getOutput()->getBaseId());
        return false;
    }
};

// Random in range <a,b)
class GenerateUniform : public Function {
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;

        // a = 0
        // b = 1

        float a = *(float*)_getInput(0)->getOutput()->getData();
        float b = *(float*)_getInput(1)->getOutput()->getData();

        float x = rand() / (float(RAND_MAX) + 1.0);
        x = x * (b - a) + a;
        *(float*)this->getOutput()->getData() = x;
        return false;
    }
};

// Random in range <0,1)
class GenerateRandom : public Function {
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;

        *(float*)this->getOutput()->getData() = rand() / (float(RAND_MAX) + 1.0);
        return false;
    }
};

class SetRandomSeed : public Function {
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;

        srand(*(int*)_getInput(0)->getOutput()->getData());
        return false;
    }
};

template <typename DEST, typename SRC>
class Cast : public Function {
public:
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;
        DEST out = (DEST) * (SRC*)_getInput(0)->getOutput()->getData();
        *(DEST*)(getOutput()->getData()) = out;
        LOG_DEBUG("Cast from %s to %s \n", keyword<SRC>().c_str(), keyword<DEST>().c_str());
        return false;
    }
};

class Construct : public Function {
public:
    size_t getCountOfComponents()
    {
        auto output = std::dynamic_pointer_cast<CompositeResource>(this->getOutput());
        return output->getComponentCount();
    }
    virtual bool operator()(RunStatus& stat)
    {
        if (_doInputs(stat))
            return true;

        LOG_DEBUG("Construct \n");
        assert(this->getOutput() != nullptr);
        auto output = std::dynamic_pointer_cast<CompositeResource>(this->getOutput());
        size_t count = output->getComponentCount();
        size_t countOfFunctionInputs = this->getCountOfInputs();

        // presume correct match of arguments and parameters
        assert(count == countOfFunctionInputs);
        static_cast<void>(countOfFunctionInputs);

        for (size_t i = 0; i < count; i++) {
            LOG_DEBUG("Construct atrib %d\n", i);
            output->getComponent(i)->copy(this->_getInput(i)->getOutput());
        }

        return false;
    }

    virtual bool bindInput(size_t id, std::shared_ptr<Function> func)
    {
        auto output = std::dynamic_pointer_cast<CompositeResource>(this->getOutput());
        assert(output != nullptr);

        if (id >= output->getComponentCount())
            return false;
        // detect type consistency
        if (!func->getOutput()->hasSameType(output->getComponent(id))) {
            return false;
        }
        Function::bindInput(id, func);
        return true;
    }
};

/*
 * Create classes for standard functions (operators)
 */

DEF_UNARY_OP(Copy, );

DEF_PREFIX_BOOL_UNARY_OP(Negation, !);
DEF_PREFIX_UNARY_OP(UnaryMinus, -);
DEF_PREFIX_UNARY_OP(UnaryPlus, +);

DEF_PREFIX_UNARY_FUNCTION(Sin, sin);
DEF_PREFIX_UNARY_FUNCTION(Cosin, cos);

DEF_BINARY_OP(Add, +);
DEF_BINARY_OP(Sub, -);
DEF_BINARY_OP(Mul, *);
DEF_BINARY_OP(Div, /);
DEF_BINARY_OP(Modulo, %);
DEF_BINARY_LOGIC_OP(Greater, >);
DEF_BINARY_LOGIC_OP(Less, <);
DEF_BINARY_LOGIC_OP(Eq, ==);
DEF_BINARY_LOGIC_OP(And, &&);
DEF_BINARY_LOGIC_OP(Or, ||);
DEF_BINARY_LOGIC_OP(NotEq, !=);
#endif
