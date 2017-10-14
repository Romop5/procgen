#include "resource.h"
#include "function.h"
#include "typereg.h"

typedef std::shared_ptr<Function> (*func_constr)();
class FunctionReg 
{
	public:
		FunctionReg(std::shared_ptr<TypeRegister> reg): tr(reg){};
		bool addFunction(std::string name,func_constr c);
		std::shared_ptr<Function> getFunc(std::string name);
		
		// Print out all info
		void _debug();
	private:
		std::shared_ptr<TypeRegister> tr;
		std::map<std::string, func_constr> func;
};
