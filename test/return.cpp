#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "typereg.h"
#include <iostream>
#include "variablereg.h"
#include "functionreg.h"
#include "std.h"
#include "statement.h"

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
	*(long*) vara->value = 10;

	auto varb= tr->sharedResource("int");
	REQUIRE(varb != nullptr);
	*(long*) varb->value = 1;

	auto output = tr->sharedResource("int");
	REQUIRE(output != nullptr);
	*(long*) output->value = 1;


	
	auto dec = fr.getFunc("tSub:int");
	REQUIRE(dec != nullptr);
	dec->inputs.push_back(vara);
	dec->inputs.push_back(varb);
	dec->output = vara;

	auto ret = std::make_shared<Return>();
	REQUIRE(ret != nullptr);
	auto body = std::make_shared<Body>();
	REQUIRE(body != nullptr);
	body->stats.push_back(dec);
	//body->stats.push_back(ret);

	auto expr = fr.getFunc("tGreater:int");
	REQUIRE(expr != nullptr);
	expr->inputs.push_back(vara);
	expr->inputs.push_back(varb);
	expr->output = output;
	auto wh= std::make_shared<While>();
	REQUIRE(wh != nullptr);
	wh->expr = expr;
	wh->stat = body;

	RunStatus stat;
	(*wh)(stat);
	std::cout << "Result: " << *(long*) vara->value << " vs " << *(long*) varb->value << std::endl;

}