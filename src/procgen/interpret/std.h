#include <procgen/interpret/typereg.h>

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

/**
* @brief Detect if two standard types are convertible to common type
* @param first
* @param second 
*
* Returns which of those two types is common (or the other one can be converted to)
* If two types are incompatbile, returns NULL (0).
* If second type is convertible to first, retursn 1.
* If first type is convertible to second, returns 2.
*/

int getCommonType(std::string first, std::string second);
