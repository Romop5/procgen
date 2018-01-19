#include "catch.hpp"
#include <stdexcept>
#include <iostream>
#include "interpret.h"

TEST_CASE("Composite")
{
	auto tr = std::make_shared<TypeRegister>();	
	registerStandardTypes(&(*tr));
	
	auto fr = std::make_shared<FunctionReg>(tr);	
	registerStandardFunctions(&(*fr));

	// register collection of ints

	TypeId base = tr->getTypeId("int");	
	tr->addCollection("collection-int",base);
	
	auto collection = std::dynamic_pointer_cast<CollectionResource>(tr->sharedResource("collection-int"));

	REQUIRE(collection != nullptr);

	// Fill up the collection with int <0,5)	
	
	auto number = tr->sharedResource("int");
	
	auto appender = std::make_shared<CollectionAppend>();	
	appender->bindInput(0, fr->getHandler(collection));
	appender->bindInput(1, fr->getHandler(number));

	RunStatus stat;
	for(int i = 0; i < 5; i++)
	{
		*(int*) number->getData() = i;
		REQUIRE((*appender)(stat) == false);
	}


	// create structure
	
	TypeId intType = tr->getTypeId("int");
	TypeId floatType  = tr->getTypeId("float");


	std::vector<TypeId> types = {intType, floatType};

	auto structType = tr->addComposite("struct test", types);
	REQUIRE(structType != 0);

	auto structVal = tr->sharedResource("struct test");
	REQUIRE(structVal != nullptr);


	auto index = tr->sharedResource("int");
	auto intVal = tr->sharedResource("int");
	auto floatVal = tr->sharedResource("float");

	auto setter = std::make_shared<CompositeSet>();
	setter->bindInput(0,fr->getHandler(structVal));
	setter->bindInput(1,fr->getHandler(index));
	setter->bindInput(2,fr->getHandler(intVal));

	/*Set intval */
	*(int*) index->getData() = 0;
	*(int*) intVal->getData() = 1337;
	*(float*) floatVal->getData() = 666.6666f;

	RunStatus rs;
	REQUIRE((*setter)(rs) == false);

	/* Set float */

	*(int*) index->getData() = 1;
	setter->bindInput(1,fr->getHandler(index));
	setter->bindInput(2,fr->getHandler(floatVal));

	REQUIRE((*setter)(rs) == false);

	/* Getter */
	auto getter = std::make_shared<CompositeGet>();
	getter->bindInput(0,fr->getHandler(structVal));
	getter->bindInput(1,fr->getHandler(index));


	*(int*) index->getData() = 0;
	*(int*) intVal->getData() = 0;

	/* Get int val */
	REQUIRE((*getter)(rs) == false);
	REQUIRE(*(int*) getter->getOutput()->getData() == 1337);
	std::cout << "Value of int: " << *(int*) getter->getOutput()->getData() << std::endl;

	/* Get float val */
	*(int*) index->getData() = 1;
	REQUIRE((*getter)(rs) == false);
	REQUIRE(abs(*(float*) getter->getOutput()->getData() - 666.666f) < 1.0f );

	std::cout << "Value of float: " << *(float*) getter->getOutput()->getData() << std::endl;




}
