#include "resource.h"
#include "typereg.h"
class VariableReg
{
	public:
		VariableReg(std::shared_ptr<TypeRegister> reg): tr(reg){};
		bool addVar(std::string name,std::shared_ptr<Resource> src);
		std::shared_ptr<Resource> getVar(std::string name);
		
		// Print out all info
		void _debug();
	private:
		std::shared_ptr<TypeRegister> tr;
		std::map<std::string, std::shared_ptr<Resource>> vars;
};
