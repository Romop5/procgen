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

namespace ProcGen {
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
    bool _hasVariableCountOfBindings;
    size_t _numberOfExpectedBindings;

    bool _doInputs(RunStatus& stat);
    std::shared_ptr<Function> _getInput(size_t id) { return this->_inputs[id]; }

public:
Function(): _hasVariableCountOfBindings(false), _numberOfExpectedBindings(0) {}
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
    
    size_t getNumberOfExpectedInputs() const { return this->_numberOfExpectedBindings; }

    bool hasRequiredBindings() const { if(_hasVariableCountOfBindings) return true; return this->_inputs.size() == this->_numberOfExpectedBindings; }

    virtual bool operator()(RunStatus& stat);


};

/**
 * @class HandleFunction
 * @brief Utility function to handle resource while passing input to Function
 */
class HandleFunction : public Function {
public:
    virtual bool operator()(RunStatus& stat);
};

/**
* @class PrintJson
* @brief Function block for debugging purposes
*/
class PrintJson : public Function {
public:
    PrintJson() { _numberOfExpectedBindings = 1; }
    virtual bool bindInput(size_t id, std::shared_ptr<Function> func);
    virtual bool operator()(RunStatus& stat);
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
    virtual bool operator()(RunStatus& stat);
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
    virtual bool operator()(RunStatus& stat);
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
    virtual bool operator()(RunStatus& stat);
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
    virtual bool operator()(RunStatus& stat);
};

/**
* @class CollectionClear
* @brief Remove all items
*
* Input 0 - collection
*/
class CollectionClear : public Function {
public:
    virtual bool operator()(RunStatus& stat);
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
    virtual bool operator()(RunStatus& stat);
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
    virtual bool operator()(RunStatus& stat);
};

// TODO: create macro type2type
class FloatToBool : public Function {
public:
    virtual bool operator()(RunStatus& stat);
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
#define DEF_BINARY_OP(OPNAME, OPERATOR)                                                                                                            \
    template <typename T>                                                                                                                          \
    class OPNAME : public Function {                                                                                                               \
    public:                                                                                                                                        \
        virtual bool operator()(RunStatus& stat)                                                                                                   \
        {                                                                                                                                          \
            if (_doInputs(stat))                                                                                                                   \
                return true;                                                                                                                       \
            T first = *(T*)_getInput(0)->getOutput()->getData();                                                                                   \
            T second = *(T*)_getInput(1)->getOutput()->getData();                                                                                  \
            if (std::string(#OPNAME) == "Div" && second == 0)                                                                                      \
                throw std::runtime_error(std::string("Division by zero: ") + std::to_string(first) + std::string(" / ") + std::to_string(second)); \
            T out = first OPERATOR second;                                                                                                         \
            LOG_DEBUG(#OPNAME ": %s = %s " #OPERATOR " %s\n", std::to_string(out).c_str(),                                                         \
                std::to_string(first).c_str(),                                                                                                     \
                std::to_string(second).c_str());                                                                                                   \
            *(T*)(getOutput()->getData()) = out;                                                                                                   \
            return false;                                                                                                                          \
        }                                                                                                                                          \
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
        OPNAME() {_numberOfExpectedBindings = 1; }                                      \
        virtual bool bindInput(size_t id, std::shared_ptr<Function> fn)                 \
        {                                                                               \
            if(fn->getOutput()->getTypeName() != keyword<T>())               \
                return false;                                                           \
            return Function::bindInput(id, fn);                                         \
        }                                                                               \
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
    virtual bool operator()(RunStatus& stat);
};

// Run-time verification of type
class Convert : public Function {
private:
    TypeId type;

public:
    Convert(TypeId _type)
        : type(_type){
            _numberOfExpectedBindings = 1;
    };
    virtual bool operator()(RunStatus& stat);
};

// Returns run-time TypeId for given resource
// TypeId is int
class GetTypeId : public Function {
    public:
    GetTypeId() { _numberOfExpectedBindings = 1; }
    virtual bool operator()(RunStatus& stat);
};

// Random in range <a,b)
class GenerateUniform : public Function {
    public:
    GenerateUniform()  {
        _numberOfExpectedBindings = (2);
    }
    virtual bool bindInput(size_t id, std::shared_ptr<Function> func);
    virtual bool operator()(RunStatus& stat);
};

// Random in range <0,1)
class GenerateRandom : public Function {
    public:
    virtual bool bindInput(size_t id, std::shared_ptr<Function> func) override;
    virtual bool operator()(RunStatus& stat);
};

class SetRandomSeed : public Function {
   public:
   SetRandomSeed() {
        _numberOfExpectedBindings = 1;
   }
   
   virtual bool bindInput(size_t id, std::shared_ptr<Function> func) override;
    virtual bool operator()(RunStatus& stat);
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

class ConstructCollection : public Function {
public:
    virtual bool operator()(RunStatus& stat);
};

class Construct : public Function {
public:
    size_t getCountOfComponents();
    virtual bool operator()(RunStatus& stat);
    virtual bool bindInput(size_t id, std::shared_ptr<Function> func);
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
DEF_PREFIX_UNARY_FUNCTION(Sqrt, sqrt);

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
}
#endif
