#ifndef _TYPESTRING_H
#define _TYPESTRING_H
#include <string>
#include "types.h"

template<class T>
std::string keyword();

/*template<>
std::string keyword<double>() {return "double";}

template<>
std::string keyword<int>() {return "int";}
*/
#define KEYWORD(type,typeName)\
template<> std::string keyword<type>() {return typeName;}

#endif
