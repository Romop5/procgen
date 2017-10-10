#include "typereg.h"
#include <iostream>

using namespace std;
int main()
{
	TypeRegister tr;
	#define REG_TYPE(type,typeName)\
		tr.add<type>(typeName);
	// Register all built int types
	FORALL_ATOMICTYPES(REG_TYPE);

	cout << tr.add<int>("int") << std::endl;

	cout << tr.getTypeId("int") << std::endl;
	cout << tr.getTypeId("int8") << std::endl;
	cout << tr.getTypeId(keyword<int>()) << std::endl;
	using newint = int;
	cout << tr.getTypeId(keyword<newint>()) << std::endl;
	cout << tr.getTypeId(keyword<double>()) << std::endl;
	cout << tr.getTypeId(keyword<float>()) << std::endl;
	
}
