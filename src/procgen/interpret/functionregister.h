/*
 * Procedural generation library
 * xdobia11 - Roman Dobias
 * Purpose: FunctionRegister registers shared<Function> under std::string name
 * Note: standard functions available under FUNC:TYPE name
 *
 */
#include <functional>
#include <procgen/interpret/compositefunction.h>
#include <procgen/interpret/function.h>
#include <procgen/interpret/resource.h>
#include <procgen/interpret/typeregister.h>

namespace ProcGen {
// function pointer = Function constructor
typedef std::shared_ptr<Function> (*func_constr)();

/**
* @class FunctionReg
* @brief Register of named functions 
*
* This class is used to register function blocks under string names. This is
* useful both for user-defined composite blocks and standard functions.
*
* Both kind of functions are instanciated using this->getFunc().
*/
class FunctionReg {
public:
    FunctionReg(std::weak_ptr<TypeRegister> reg)
        : tr(reg){};
    ~FunctionReg(){};
    /**
     * @brief Register function provided name and lambda constructor
     *
     * @param name function name
     * @param std::function Function constructor
     *
     * @return 
     */
    bool addFunction(std::string name, std::function<std::shared_ptr<Function>()>);

    /**
     * @brief Register class as function
     *
     * @tparam X class name
     * @param name function name in interpret
     *
     * @return false if name is already defined
     */
    template <class X>
    bool addFunction(std::string name)
    {
        return this->addFunction(name, [] { return std::static_pointer_cast<Function>(std::make_shared<X>()); });
    }

    /**
     * @brief Register function from statements
     *
     * @param name function name
     * @param body function statements
     * @param inputs function parameters resources
     * @param output function return value resource
     *
     * @return 
     */
    bool addCompositeFunction(
        std::string name,
        std::shared_ptr<Statement> body,
        std::vector<std::shared_ptr<Resource>> inputs,
        std::shared_ptr<Resource> output)
    {
        auto cfs = std::make_shared<CompositeFunction>(body, inputs, output);
        // register lambda constructor for given composite function
        func[name] = [cfs, name] { return std::static_pointer_cast<Function>(std::make_shared<FunctionCall>(cfs, name)); };
        return true;
    }

    /**
     * @brief Get instance of function by name
     *
     * @param name function name
     *
     * @return  Function instance or nullptr if function misses 
     */
    std::shared_ptr<Function> getFunc(std::string name);

    /**
* @brief Returns an instance of utility funcion handling given resource
* @param res
*/
    std::shared_ptr<HandleFunction> getHandler(std::shared_ptr<Resource> res)
    {
        auto hf = std::make_shared<HandleFunction>();
        hf->bindOutput(res);
        return hf;
    }

    // Print out all info
    void _debug();
    std::weak_ptr<TypeRegister> getTypeRegister() { return tr; }

private:
    std::weak_ptr<TypeRegister> tr;
    std::map<std::string, std::function<std::shared_ptr<Function>()>> func;
};
}
