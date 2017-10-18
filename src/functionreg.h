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

// function pointer = Function constructor
typedef std::shared_ptr<Function> (*func_constr)();
class FunctionReg 
{
	public:
		FunctionReg(std::shared_ptr<TypeRegister> reg): tr(reg){};
		bool addFunction(std::string name,std::function<std::shared_ptr<Function>()>);
		//bool addFunction(std::string name,func_constr c);

		// Registers class X constructor under 'name'
		template<class X>
		bool addFunction(std::string name)
		{
			this->addFunction(name, []{return std::static_pointer_cast<Function>(std::make_shared<X>());});
		}
		std::shared_ptr<Function> getFunc(std::string name);
		
		// Print out all info
		void _debug();
		std::shared_ptr<TypeRegister> getTypeRegister() {return tr;}
	private:
		std::shared_ptr<TypeRegister> tr;
		//std::map<std::string, func_constr> func;
		std::map<std::string, std::function<std::shared_ptr<Function>()>> func;
};
