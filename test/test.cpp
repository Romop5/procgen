#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "function.h"
//#include "statement.h"
//#include "single.h"

#include "typereg.h"
#include "types.h"

/*
TEST_CASE("Testing new TypeRegister")
{
	auto tr = TypeRegister();
	#define REG_TYPE(TYPE,TYPENAME)\
	tr.add<TYPE>(TYPENAME);
	FORALL_ATOMICTYPES(REG_TYPE);

	auto a = tr.sharedResource(tr.getTypeId(keyword<int>()));
	auto box = Add<int>();
	box.inputs.push_back(a);
	box.inputs.push_back(a);
	auto res = tr.sharedResource(tr.getTypeId(keyword<int>()));
	box.output = res;

	*(int*) a->value = 666;
	RunStatus stat;
	box(stat);

	REQUIRE(*res == 1332);
	//REQUIRE(*(int*) res->value == 1332);

	auto mulbox = tMul<int>();
	mulbox.inputs.push_back(a);
	mulbox.inputs.push_back(a);
	mulbox.output = res;
	mulbox(stat);
	REQUIRE(*res == 443556);

}



// create box for a+a
TEST_CASE("Create box for result = a + a, where a = 666;")
{
	auto fct = ResourceFactory();
	auto a = fct.createResource("a");
	auto box = Add<int>();
	box.inputs.push_back(a);
	box.inputs.push_back(a);
	auto res = fct.createResource("result");
	box.output = res;

	*(int*) a->value = 666;
	RunStatus stat;
	box(stat);

	REQUIRE(*(int*) res->value == 1332);

}
TEST_CASE("Create float box for result = a + a, where a = 666;")
{
	auto fct = ResourceFactory();
	auto a = fct.createResource("a");
	auto box = Add<float>();
	box.inputs.push_back(a);
	box.inputs.push_back(a);
	auto res = fct.createResource("result");
	box.output = res;

	*(float*) a->value = 666.01;
	RunStatus stat;
	box(stat);

	float r = (*(float*) (res->value))-1332;
	REQUIRE(r < 0.2f);

}

TEST_CASE("Char test")
{
	auto fct = ResourceFactory();
	auto a = fct.createResource("a");
	auto box = Add<char>();
	box.inputs.push_back(a);
	box.inputs.push_back(a);
	auto res = fct.createResource("result");
	box.output = res;

	*(char*) a->value = 130;
	RunStatus stat;
	box(stat);

	float r = (*(float*) (res->value))-1332;
	REQUIRE(r < 0.2f);

}

/*
TEST_CASE("create box for (a+a)*10")
{
	auto fct = ResourceFactory();
	auto res = fct.createResource("result");
	auto a = fct.createResource("a");
	*(int*) a->value = 666;
	auto plus = std::make_shared<Add>();
	RunStatus stat;
	auto mul = Mul();
	plus->inputs.push_back(a);
	plus->inputs.push_back(a);
	std::shared_ptr<Resource> c10 = std::make_shared<Resource>();
	c10->value = new unsigned char[sizeof(int)];

	*(int*) c10->value = 10;

	plus->output = res;

	mul.inputs.push_back(res);
	mul.inputs.push_back(c10);

	mul.children.push_back(plus);

	
	mul.output = fct.createResource("mulResult");

	mul(stat);

	// (a+a)*10 => (666+666)*2 = 13320
	REQUIRE(*(int*) mul.output->value == 13320);
}

TEST_CASE("Try while")
{
	// {
	// a = 10;
	// b = 5;
	// while(a>b) b++;
	// }
	auto fct = ResourceFactory();
	auto res = fct.createResource("result");
	// var a
	auto a = fct.createResource("a");
	*(int*) a->value = 10;
	// var b
	auto b = fct.createResource("b");
	*(int*) b->value = 5;

	// A > B
	auto condition = std::make_shared<Greater>();
	condition->inputs.push_back(a);
	condition->inputs.push_back(b);
	condition->output = res;
	// A++
	auto c1 = fct.createResource("c1");
	*(int*) c1->value = 1;
	auto inc = std::make_shared<Add>();
	inc->inputs.push_back(b);
	inc->inputs.push_back(c1);
	inc->output = b;
	// body
	auto bd = std::make_shared<Body>();
	bd->stats.push_back(inc);
	auto dbg = std::make_shared<DebugStatement>();
	dbg->inputs.push_back(b);
	bd->stats.push_back(dbg);
	// While
	RunStatus stat;
	auto wh = While();
	wh.expr = condition;
	wh.stat = bd;
	(wh)(stat);

	REQUIRE(*(int*) b->value == 10);
	fct.dump();
}

int fib(int n)
{
	auto res = ResourceFactory();
	auto vn = res.createResource("n");
	*(int*) vn->value = n;
	auto first = res.createResource("first");
	*(int*) first->value = 1;
	auto second= res.createResource("second");
	*(int*) second->value = 1;
	auto result= res.createResource("result");
	auto exprresult = res.createResource("exprresult");
	auto cnt = res.createResource("cnt");
	*(int*) cnt->value = 2;

	// cntr < n
	auto expr = std::make_shared<Greater>();
	expr->inputs.push_back(vn);
	expr->inputs.push_back(cnt);
	expr->output = exprresult;
	// cntr++
	auto inc = std::make_shared<Add>();
	auto c1 = res.createResource("const1");
	*(int*) c1->value = 1;
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

	RunStatus stat;
	auto body = Body();
	body.stats.push_back(wh);	// while(cntr < n) ...	
	body.stats.push_back(cp3);	// result = second;
	
	body(stat);
	return *(int*)result->value;
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
}*/
