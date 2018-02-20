#include "catch.hpp"
#include <stdexcept>
#include <iostream>
#include <procgen/interpret/interpret.h>

TEST_CASE("Collections")
{
	auto tr = std::make_shared<TypeRegister>();	
	registerStandardTypes(&(*tr));
	
	auto fr = std::make_shared<FunctionReg>(tr);	
	registerStandardFunctions(&(*fr));

	// register collection of ints

	TypeId base = tr->getTypeId("int");	
	tr->addCollection();
	
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

	// Check all items
	for(int i = 0; i < 5; i++)
	{
		REQUIRE(*(int*) collection->at(i)->getData() == i);
		std::cout << "Coll: " << i << " - " << *(int*) collection->at(i)->getData() << std::endl;
	}

	auto index = tr->sharedResource("int");
	auto getter = std::make_shared<CollectionIndex>();	
	getter->bindInput(0, fr->getHandler(collection));
	getter->bindInput(1, fr->getHandler(index));

	for(int i = 0; i < 5; i++)
	{
		*(int*) index->getData() = i;
		(*getter)(stat);
		std::cout << "Coll vol2: " << i << " - " << *(int*) number->getData() << std::endl;
		REQUIRE(*(int*) getter->getOutput()->getData() == i);
	}

}
