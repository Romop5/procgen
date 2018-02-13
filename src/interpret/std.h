#include "typereg.h"
	
/**
* @brief Register std types 
* @param tr
*
* Register standard types to iterpret. The types include int, float, char, bool
* and are neccessary to construct user types.
*/
void registerStandardTypes(TypeRegister* tr);

/**
* @brief Register standard functions
* @param fr
*
* Register functions based on standard types. These are elementary function
* that are required to manipulate with standard types and their values.
* For instance, registers functions for operators +,-, =, ==, !=, <, >
*/
void registerStandardFunctions(FunctionReg* fr);

