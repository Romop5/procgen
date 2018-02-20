#include <procgen/interpret/std.h>
#include <procgen/interpret/typereg.h>
#include <procgen/interpret/variablereg.h>
#include <procgen/interpret/functionreg.h>

template<class X>
int fib(int n)
{
	// Set basic type 
	std::string type = keyword<X>;
	
	// TypeRegister to handle all types
	auto tr = std::make_shared<TypeRegister>();

	registerStandardTypes(&(*tr));

	// VariableRegister for this types
	auto vr = std::make_shared<VariableReg>(tr);

	auto vn = vr->addVar("n", tr->sharedResource(type));

	*(int*) vn->getData() = n;
	auto first = vr->addVar("first", tr->sharedResource(type));
	*(int*) first->getData() = 1;
	auto second= vr->addVar("second", tr->sharedResource(type));
	*(int*) second->getData() = 1;
	auto result = vr->addVar("result", tr->sharedResource(type));
	auto exprresult= vr->addVar("exprresult", tr->sharedResource(type));
	auto exprresult = res.createResource("exprresult");
	auto cnt= vr->addVar("cnt", tr->sharedResource(type));
	*(int*) cnt->getData() = 2;

	auto fr = std::make_shared<FunctionRegister>();

	registerStandardFunctions(&(*fr));

	// cntr < n
	auto expr = std::make_shared<Greater>();
	expr->inputs.push_back(vn);
	expr->inputs.push_back(cnt);
	expr->output = exprresult;
	// cntr++
	auto inc = std::make_shared<Add>();
	auto c1 = res.createResource("const1");
	*(int*) c1->getData() = 1;
	inc->inputs.push_back(c1);
	inc->inputs.push_back(cnt);
	inc->output = cnt;

	//int tmp = first + second;
	auto tmp = res.createResource("tmp");
	auto sum = std::make_shared<Add>();
	sum->inputs.push_back(first);
	sum->inputs.push_back(second);
	sum->output = tmp;

	// first = second;
	auto cp1 = std::make_shared<Copy>();
	cp1->inputs.push_back(second);
	cp1->output = first;

	// second = tmp;
	auto cp2 = std::make_shared<Copy>();
	cp2->inputs.push_back(tmp);
	cp2->output = second;
	// result = second;
	auto cp3 = std::make_shared<Copy>();
	cp3->inputs.push_back(second);
	cp3->output = result;

	// finish it !
	auto bodywhile = std::make_shared<Body>();
	bodywhile->stats.push_back(inc); // cntr++
	bodywhile->stats.push_back(sum); // int tmp = ...
	bodywhile->stats.push_back(cp1); // first = second;
	bodywhile->stats.push_back(cp2); // second = tmp;

	auto wh = std::make_shared<While>();
	wh->expr = expr;
	wh->stat = bodywhile;

	auto body = Body();
	body.stats.push_back(wh);	// while(cntr < n) ...	
	body.stats.push_back(cp3);	// result = second;
	
	body();
	return *(int*)result->getData();
}

TEST_CASE("Fibbonaci")
{
	// {
	// n = nth
	// int first = 1;
	// int second = 1;
	// int result = 1;
	// int cntr = 2;
	// while(cntr < n)
	// {
	// 	cntr++;
	// 	int tmp = first + second;
	// 	first = second;
	// 	second = tmp;
	// }
	// result = second;

	REQUIRE(fib(1) == 1);
	REQUIRE(fib(2) == 1);
	REQUIRE(fib(3) == 2);
	REQUIRE(fib(4) == 3);
	REQUIRE(fib(5) == 5);
	REQUIRE(fib(6) == 8);
	REQUIRE(fib(7) == 13);
	REQUIRE(fib(44) == 701408733);
}
