#include "catch.hpp"
#include <procgen/interpret/interpret.h>

TEST_CASE("TypeRegister - standard types presence")
{

    auto tr = std::make_shared<TypeRegister>();
    registerStandardTypes(tr.get());


    REQUIRE(tr->getTypeId("int") != 0);
    REQUIRE(tr->getTypeId("float") != 0);
    REQUIRE(tr->getTypeId("char") != 0);

    REQUIRE(tr->getTypeId("int") == tr->getTypeId(keyword<int>()));
}


TEST_CASE("TypeRegister - adding new type")
{

    auto tr = std::make_shared<TypeRegister>();
    registerStandardTypes(tr.get());

	TypeId intType = tr->getTypeId("int");
    REQUIRE(intType != 0);
	TypeId floatType  = tr->getTypeId("float");
    REQUIRE(floatType != 0);


	std::vector<TypeId> types = {intType, floatType};

	auto structType = tr->addComposite("struct test", types);
	REQUIRE(structType != 0);

	auto structVal = tr->sharedResource("struct test");
	REQUIRE(structVal != nullptr);
}



TEST_CASE("TypeRegister - creating a structured type from invalid types")
{

    auto tr = std::make_shared<TypeRegister>();
    registerStandardTypes(tr.get());

	TypeId intType = 0;
	TypeId floatType  = -1;

	std::vector<TypeId> types = {intType, floatType};

    // This must fail due to invalid types
	auto structType = tr->addComposite("struct test", types);
	REQUIRE(structType == false);

	auto structVal = tr->sharedResource("struct test");
	REQUIRE(structVal == nullptr);
}

