#include "compositefunction.h"
#include "functionreg.h"
#include "std.h"
#include <stdexcept>
#include <iostream>


std::shared_ptr<Function> createCopy(std::shared_ptr<FunctionReg> fr,
		std::string type,std::shared_ptr<Resource> src, std::shared_ptr<Resource> dst)
{
	auto copy= fr->getFunc("tCopy:"+type);
	if(copy == nullptr)
		throw std::runtime_error("tCopy not found");
	copy->inputs.push_back(src);
	copy->output = dst;
	return copy;
}

//std::shared_ptr<FunctionCall> 
void fib(std::shared_ptr<TypeRegister> tr, std::shared_ptr<FunctionReg> fr)
{
	/* 
	 * INTERFACE
	 * Input: n
	 * Output: Fib(n)
	 * See ../tests/test.cpp 
	 */

	/*
	 * Build expression tree
	 */

	// input variable n 
	auto vn = tr->sharedResource("long");

	auto const_one= tr->sharedResource("long");
	*(long*) const_one->value = 1;

	auto const_two= tr->sharedResource("long");
	*(long*) const_two->value = 2;

	// int first
	auto first = tr->sharedResource("long");
	// first = const_one
	auto copyFirst = createCopy(fr,"long",const_one,first);
	
	// int second 
	auto second = tr->sharedResource("long");
	// second = const_one
	auto copySecond = createCopy(fr,"long",const_one,second);
	
	// int result 
	auto result = tr->sharedResource("long");
	// second = const_one
	auto copyResult= createCopy(fr,"long",const_one,result);
	
	// int cnt 
	auto cnt= tr->sharedResource("long");
	// second = const_two_
	auto copyCnt= createCopy(fr,"long",const_two,cnt);


	// maybe should be bool
	auto exprresult= tr->sharedResource("long");

	// cntr < n
	auto expr = fr->getFunc("tGreater:long");
	expr->inputs.push_back(vn);
	expr->inputs.push_back(cnt);
	expr->output = exprresult;

	// cntr++
	auto inc = fr->getFunc("tAdd:long");

	inc->inputs.push_back(const_one);
	inc->inputs.push_back(cnt);
	inc->output = cnt;

	//int tmp = first + second;
	// tmp
	auto tmp = tr->sharedResource("long");
	auto sum = fr->getFunc("tAdd:long");
	//auto sum = std::make_shared<Add>();
	sum->inputs.push_back(first);
	sum->inputs.push_back(second);
	sum->output = tmp;

	// first = second;
	//auto cp1 = std::make_shared<Copy>();
	auto cp1 = fr->getFunc("tCopy:long");
	cp1->inputs.push_back(second);
	cp1->output = first;

	// second = tmp;
	auto cp2 = fr->getFunc("tCopy:long");
	cp2->inputs.push_back(tmp);
	cp2->output = second;
	// result = second;
	auto cp3 = fr->getFunc("tCopy:long");
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

	auto body = std::make_shared<Body>();
	/*
	 * Function intro
	 */
	// n = nth
	body->stats.push_back(copyFirst);	// int first = 1;
	body->stats.push_back(copySecond);	// int second = 1;
	body->stats.push_back(copyResult);	// int result = 1;
	body->stats.push_back(copyCnt);		// int cntr = 2;
	
	/*
	 * Real body of function
	 */
	body->stats.push_back(wh);	// while(cntr < n) ...	
	body->stats.push_back(cp3);	// result = second;
	
	//body();
	//return *(long*)result->value;
	
	/* Bound interface */

	//std::shared_ptr<CompositeFunction> cf = std::make_shared<CompositeFunction>(body, {vn}, result);
	//return std::make_shared<FunctionCall>(cf);
	//return addCompositeFunction("fibFce",body, {vn}, result);
	fr->addCompositeFunction("fibFce", body,{vn}, result);
}

int main()
{
	auto tr = std::make_shared<TypeRegister>();	
	registerStandardTypes(&(*tr));
	
	auto fr = std::make_shared<FunctionReg>(tr);	
	registerStandardFunctions(&(*fr));

	// create fibbonnaci prototype
	fib(tr,fr);

	auto fib_fce = fr->getFunc("fibFce");

	auto n = tr->sharedResource("long");
	auto output = tr->sharedResource("long");

	fib_fce->inputs.push_back(n);
	fib_fce->output = output;


	for(long i = 0; i < 100; i++)
	{
		*(long*) n->getData() = i;
		(*fib_fce)();
		std::cout << "Result: "<<*(long*) output->getData() << std::endl;
	}

	return 0;
}
