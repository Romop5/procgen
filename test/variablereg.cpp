#include "catch.hpp"
#include <iostream>
#include <procgen/interpret/typereg.h>
#include <procgen/interpret/variablereg.h>
#include <procgen/interpret/functionreg.h>
#include <procgen/interpret/std.h>

using namespace std;
TEST_CASE("Testing TypeRegister with STD func")
{
	std::shared_ptr<TypeRegister> tr = std::make_shared<TypeRegister>() ;

	REQUIRE(tr->getTypeId("int") == 0);	
	REQUIRE(tr->getTypeId("bool") == 0);	
	REQUIRE(tr->getTypeId("int32") == 0);	
	
	// Register all standard C++ types
	registerStandardTypes(&(*tr));

	REQUIRE(tr->getTypeId("int") != 0);	
	REQUIRE(tr->getTypeId("bool") != 0);	
	REQUIRE(tr->getTypeId("int32") == 0);	
	

	auto vr = VariableReg(tr);
	vr.addVar("a", tr->sharedResource("int"));

	auto fr = FunctionReg(tr);	
	registerStandardFunctions(&fr);
	std::cout << "Debug" <<std::endl;
		

	auto addBox = fr.getFunc("Add:int");
	auto box = fr.getFunc("Mul:int");
	
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


	box->bindInput(0, fr.getHandler(a));
	box->bindInput(1, fr.getHandler(b));
	box->bindOutput(result);

	RunStatus stat;
	bool interpretationResult = (*box)(stat);


	int r = *(int*) result->getData();
	std::cout << *(int*) a->getData() << " OP " << *(int*) b->getData()  << std::endl;
	std::cout << "Result: " << r << std::endl;
	REQUIRE(r == 500);
}
