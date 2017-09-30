#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "single.h"

// create box for a+a
TEST_CASE("Create box for result = a + a, where a = 666;")
{
	auto fct = ResourceFactory();
	auto a = fct.createResource("a");
	auto box = Add();
	box.inputs.push_back(a);
	box.inputs.push_back(a);
	auto res = fct.createResource("result");
	box.output = res;

	*(int*) a->value = 666;
	box();

	REQUIRE(*(int*) res->value == 1332);

}

TEST_CASE("create box for (a+a)*10")
{
	auto fct = ResourceFactory();
	auto res = fct.createResource("result");
	auto a = fct.createResource("a");
	*(int*) a->value = 666;
	auto plus = std::make_shared<Add>();
	auto mul = Mul();
	plus->inputs.push_back(a);
	plus->inputs.push_back(a);
	std::shared_ptr<Resource> c10 = std::make_shared<Resource>();
	c10->value = new int();

	*(int*) c10->value = 10;

	plus->output = res;

	mul.inputs.push_back(res);
	mul.inputs.push_back(c10);

	mul.children.push_back(plus);

	
	mul.output = fct.createResource("mulResult");

	mul();

	// (a+a)*10 => (666+666)*2 = 13320
	REQUIRE(*(int*) mul.output->value == 13320);
}
