#ifndef _TYPESTRING_H
#define _TYPESTRING_H
#include <procgen/interpret/types.h>
#include <string>

namespace ProcGen {
// Provides an easy way to iterate a FNC function over standard built-in types
#define FORALL_ATOMICTYPES(FNC) \
    FNC(int, "int");            \
    FNC(bool, "bool");          \
    FNC(float, "float");        \
    FNC(long, "long");
/*
	FNC(char,"char");\
	FNC(short,"short");\
	FNC(double,"double");\
*/

// Provides an easy way to iterate a FNC function over standard built-in types
// This time supports a parameter, passed to function
#define FORALL_ATOMICTYPES3(FNC, PARAM) \
    FNC(int, PARAM);                    \
    FNC(long, PARAM);                   \
    FNC(float, PARAM);

//FNC(bool,PARAM);

/*
	FNC(double,"double",PARAM);\
	FNC(char,"char",PARAM);\
	FNC(short,"short",PARAM);
*/

// Defines a templated function which can be used to convert C++ type
// to its string representation.
// A notable advantage against classic C macro is the fact that
// typedef-ed types are recognized correctly
// For example, typedef newType uint; keyword<newtype> would return uint
//
template <class T>
std::string keyword();

// Defines a simple macro to specify keyword<TYPE> for all
// our supported types
#define KEYWORD(type, typeName) \
    template <>                 \
    std::string keyword<type>() { return typeName; }
}
#endif // Calls FNC(type, string:typeName)
