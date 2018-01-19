#include "catch.hpp"
#include <stdexcept>
#include <iostream>
#include "interpret.h"

/*
 * A test of accessing struct components by component name
 */
TEST_CASE("Composite access using component name")
{
	auto tr = std::make_shared<TypeRegister>();	
	registerStandardTypes(tr.get());
	
	auto fr = std::make_shared<FunctionReg>(tr);	
	registerStandardFunctions(fr.get());

/*
 *	struct mix:
 *		int a;
 *		float b;
 *		int c;
 */
	TypeId typeInt = tr->getTypeId("int");	
	TypeId typeFloat = tr->getTypeId("float");	

	TypeId composed = tr->addCompositeWithNames("mix", {typeInt,typeFloat,typeInt},{"a","b","c"});
	REQUIRE(composed != 0);

	// Test member ship
	
	auto typeDescription = std::dynamic_pointer_cast<CompositeType>(
			tr->getType(composed));
	REQUIRE(typeDescription->hasComponentWithName("a"));
	REQUIRE(typeDescription->hasComponentWithName("b"));
	REQUIRE(typeDescription->getComponentPositionByName("a") != -1);
	REQUIRE(typeDescription->getComponentPositionByName("b") != -1);
	REQUIRE(typeDescription->getComponentPositionByName("a") == 0);
	REQUIRE(typeDescription->getComponentPositionByName("b") == 1);


	auto randomInt = tr->sharedResource("int");
	REQUIRE(randomInt);

	*(int*) randomInt->getData() = 1337;

	auto index = tr->sharedResource("int");
	// position 
	*(int*) index->getData() = 2;
	
	auto composite = tr->sharedResource("mix");
	auto setter = std::make_shared<CompositeSet>();
	// composite[index] = randomInt
	setter->bindInput(0, fr->getHandler(composite));
	setter->bindInput(1, fr->getHandler(index));
	setter->bindInput(2, fr->getHandler(randomInt));

	RunStatus rs;
	(*setter)(rs);

	// now let's verify if we can reach the randomInt using its string name
	
	*(int*) index->getData() = typeDescription->getComponentPositionByName("c");


	auto getter = std::make_shared<CompositeGet>();
	getter->bindInput(0, fr->getHandler(composite));
	getter->bindInput(1, fr->getHandler(index));

	*(int*) randomInt->getData() = 0;

	(*getter)(rs);
	REQUIRE(*(int*) getter->getOutput()->getData() == 1337);


}
