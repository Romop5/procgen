#include "resource.h"
#include "typereg.h"
/**
* @class VariableReg 
* @brief Registers and returns named variables 
*
* The responsibility of this class is to register variables
* and allow look-up.
* Instances of this class are used to hold global variables
* or stack-frame variables.
*/
class VariableReg
{
	public:
		VariableReg(std::shared_ptr<TypeRegister> reg): tr(reg){};
/**
* @brief Register a named variable
*
* @param name The name
* @param src The instance of type, paired with name
*
* @return FALSE if variable already exists
*/
		bool addVar(std::string name,std::shared_ptr<Resource> src);
/**
* @brief Get reference to registered variable
*
* @param name
*
* @return nullptr if name doesn't exist
*/
		std::shared_ptr<Resource> getVar(std::string name);
		
/**
* @brief Print out all bindings
*/
		void _debug();
/**
* @brief Remove all registrations
*/
		void clear();
	private:
		std::shared_ptr<TypeRegister> tr;
        
        /// holds all variable name to resource mappings
		std::map<std::string, std::shared_ptr<Resource>> vars;
};
