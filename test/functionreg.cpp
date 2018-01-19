#include "catch.hpp"
#include "interpret.h"

TEST_CASE("FunctionReg - basics")
{

    auto tr = std::make_shared<TypeRegister>();
    auto fr = std::make_shared<FunctionReg>(tr);

    registerStandardTypes(tr.get());
    registerStandardFunctions(fr.get());


    // Check up standard functions
    REQUIRE(fr->getFunc(std::string("Add:int")) != nullptr);
    REQUIRE(fr->getFunc(std::string("Add:float"))!= nullptr);
    REQUIRE(fr->getFunc(std::string("Mul:float"))!= nullptr);
    REQUIRE(fr->getFunc(std::string("Mul:int"))!= nullptr);

    // Try to override standard function => should result with error

    REQUIRE(fr->addFunction<Mul<int>>(std::string("Mul:int"))== false);
    REQUIRE(fr->addFunction<Add<int>>(std::string("Add:int"))== false);
}


