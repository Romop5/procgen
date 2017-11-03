#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "typereg.h"
#include <iostream>
#include "variablereg.h"
#include "functionreg.h"
#include "std.h"

using namespace std;
TEST_CASE("Testing TypeRegister with STD func")
{
	std::shared_ptr<TypeRegister> tr = std::make_shared<TypeRegister>() ;

	REQUIRE(tr->getTypeId("int") == 0);	
	REQUIRE(tr->getTypeId("char") == 0);	
	REQUIRE(tr->getTypeId("bool") == 0);	
	REQUIRE(tr->getTypeId("int32") == 0);	
	
	// Register all standard C++ types
	registerStandardTypes(&(*tr));

	REQUIRE(tr->getTypeId("int") != 0);	
	REQUIRE(tr->getTypeId("char") != 0);	
	REQUIRE(tr->getTypeId("bool") != 0);	
	REQUIRE(tr->getTypeId("int32") == 0);	
	

	auto vr = VariableReg(tr);
	vr.addVar("a", tr->sharedResource("int"));

	auto fr = FunctionReg(tr);	
	registerStandardFunctions(&fr);
	std::cout << "Debug" <<std::endl;
		

	auto addBox = fr.getFunc("tAdd:int");
	auto box = fr.getFunc("tMul:int");
	
	REQUIRE(addBox!= nullptr);	
	REQUIRE(box != nullptr);	

	vr.addVar("a", tr->sharedResource("int"));
	vr.addVar("b", tr->sharedResource("int"));
	vr.addVar("result", tr->sharedResource("int"));

	auto a = vr.getVar("a");
	auto b = vr.getVar("b");
	auto result = vr.getVar("result");

	REQUIRE(a != nullptr);	
	REQUIRE(b != nullptr);	
	REQUIRE(result != nullptr);	

	*(int*) a->getData() = 100;
	*(int*) b->getData() = 5;

	box->inputs.push_back(a);
	box->inputs.push_back(b);
	box->output = result;

	RunStatus stat;
	bool interpretationResult = (*box)(stat);


	int r = *(int*) result->getData();
	std::cout << *(int*) a->getData() << " OP " << *(int*) b->getData()  << std::endl;
	std::cout << "Result: " << r << std::endl;
	REQUIRE(r == 500);
}
