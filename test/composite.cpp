#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "typereg.h"
#include <iostream>
#include "variablereg.h"
#include "functionreg.h"
#include "std.h"
#include "statement.h"

using namespace std;
TEST_CASE("Testing TypeRegister with STD func")
{
	std::shared_ptr<TypeRegister> tr = std::make_shared<TypeRegister>() ;
	// Register all standard C++ types
	registerStandardTypes(&(*tr));


	auto vr = VariableReg(tr);
	vr.addVar("a", tr->sharedResource("int"));

	auto fr = FunctionReg(tr);	
	registerStandardFunctions(&fr);
		

	/*
	 * struct vec3
	 * 	float x,y,z
	 */

	auto int_type = tr->getTypeId("int");

	std::vector<TypeId> types = {int_type,int_type,int_type};

	auto vec3_type = tr->addComposite("vec3",types);
	auto vec3_inst = tr->sharedResource(vec3_type);

	auto z = tr->sharedResource("int");
	auto a = tr->sharedResource("int");
	auto b = tr->sharedResource("int");
	auto c = tr->sharedResource("int");
	*(int*) z->getData() = 0;
	*(int*) a->getData() = 1;
	*(int*) b->getData() = 2;
	*(int*) c->getData() = 3;

	auto list = std::make_shared<Body>();

	auto seta = fr.getFunc("set");
	seta->output = vec3_inst;
	seta->inputs.push_back(a);
	seta->inputs.push_back(z);
	list->stats.push_back(seta);

	auto setb = fr.getFunc("set");
	setb->output = vec3_inst;
	setb->inputs.push_back(b);
	setb->inputs.push_back(a);
	list->stats.push_back(setb);

	auto setc = fr.getFunc("set");
	setc->output = vec3_inst;
	setc->inputs.push_back(c);
	setc->inputs.push_back(b);
	list->stats.push_back(setc);

	RunStatus stat;
	(*list)(stat);

}
