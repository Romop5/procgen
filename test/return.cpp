#include "catch.hpp"
#include <procgen/interpret/typereg.h>
#include <iostream>
#include <procgen/interpret/variablereg.h>
#include <procgen/interpret/functionreg.h>
#include <procgen/interpret/std.h>
#include <procgen/interpret/statement.h>

using namespace std;
TEST_CASE("Testing return")
{
	std::shared_ptr<TypeRegister> tr = std::make_shared<TypeRegister>() ;
	// Register all standard C++ types
	registerStandardTypes(&(*tr));

	auto fr = FunctionReg(tr);	
	registerStandardFunctions(&fr);
		

	auto vara= tr->sharedResource("int");
	REQUIRE(vara != nullptr);
	*(long*) vara->getData() = 10;

	auto varb= tr->sharedResource("int");
	REQUIRE(varb != nullptr);
	*(long*) varb->getData() = 1;

	auto output = tr->sharedResource("int");
	REQUIRE(output != nullptr);
	*(long*) output->getData() = 1;


	
	auto dec = fr.getFunc("Sub:int");
	REQUIRE(dec != nullptr);
	dec->bindInput(0,fr.getHandler(vara));
	dec->bindInput(1,fr.getHandler(varb));
	dec->bindOutput(vara);

	auto ret = std::make_shared<Return>();
	REQUIRE(ret != nullptr);
	auto body = std::make_shared<Body>();
	REQUIRE(body != nullptr);
	body->appendStatement(dec);

	auto expr = fr.getFunc("Greater:int");
	REQUIRE(expr != nullptr);
	expr->bindInput(0,fr.getHandler(vara));
	expr->bindInput(1,fr.getHandler(varb));
	expr->bindOutput(output);
	auto wh= std::make_shared<While>();
	REQUIRE(wh != nullptr);
    wh->bindCondition(expr);
    wh->bindStatement(body);

	RunStatus stat;
	(*wh)(stat);
	std::cout << "Result: " << *(long*) vara->getData() << " vs " << *(long*) varb->getData() << std::endl;

}
