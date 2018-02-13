/*
 * Procedural generation library
 * xdobia11 - Roman Dobias
 * Purpose: FunctionRegister registers shared<Function> under std::string name
 * Note: standard functions available under FUNC:TYPE name
 *
 */
#include "resource.h"
#include "function.h"
#include "typereg.h"
#include <functional>
#include "compositefunction.h"

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
class FunctionReg 
{
	public:
		FunctionReg(std::shared_ptr<TypeRegister> reg): tr(reg){};
		bool addFunction(std::string name,std::function<std::shared_ptr<Function>()>);

		template<class X>
		bool addFunction(std::string name)
		{
		    return this->addFunction(name, []{return std::static_pointer_cast<Function>(std::make_shared<X>());});
		}


		bool addCompositeFunction(
				std::string name,
				std::shared_ptr<Statement> core,
				std::vector<std::shared_ptr<Resource>> inputs, 
				std::shared_ptr<Resource> output)
		{
			auto cfs = std::make_shared<CompositeFunction>(core,inputs, output);
			//return std::make_shared<FunctionCall>(cfs);
			func[name] = [cfs]{return std::static_pointer_cast<Function>(std::make_shared<FunctionCall>(cfs));};
			return true;
		}

		std::shared_ptr<Function> getFunc(std::string name);

/**
* @brief Returns an instance of funcion handling given resource
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
		std::shared_ptr<TypeRegister> getTypeRegister() {return tr;}
	private:
		std::shared_ptr<TypeRegister> tr;
		std::map<std::string, std::function<std::shared_ptr<Function>()>> func;
};
