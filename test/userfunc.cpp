#include "compositefunction.h"
#include "functionreg.h"
#include "std.h"
#include <stdexcept>
#include <iostream>


std::shared_ptr<Function> createCopy(std::shared_ptr<FunctionReg> fr,
		std::string type,std::shared_ptr<Resource> src, std::shared_ptr<Resource> dst)
{
	auto copy= fr->getFunc("Copy:"+type);
	if(copy == nullptr)
		throw std::runtime_error("Copy not found");
	copy->bindInput(0,fr->getHandler(src));
	copy->bindOutput(dst);
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
	*(long*) const_one->getData() = 1;

	auto const_two= tr->sharedResource("long");
	*(long*) const_two->getData() = 2;

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
	auto expr = fr->getFunc("Greater:long");
	expr->bindInput(0, fr->getHandler(vn));
	expr->bindInput(1, fr->getHandler(cnt));
	expr->bindOutput(exprresult);

	// cntr++
	auto inc = fr->getFunc("Add:long");

	inc->bindInput(0, fr->getHandler(const_one));
	inc->bindInput(1, fr->getHandler(cnt));
	inc->bindOutput(cnt);

	//int tmp = first + second;
	// tmp
	auto tmp = tr->sharedResource("long");
	auto sum = fr->getFunc("Add:long");
	//auto sum = std::make_shared<Add>();
	sum->bindInput(0, fr->getHandler(first));
	sum->bindInput(1, fr->getHandler(second));
	sum->bindOutput(tmp);

	// first = second;
	//auto cp1 = std::make_shared<Copy>();
	auto cp1 = fr->getFunc("Copy:long");
	cp1->bindInput(0, fr->getHandler(second));
	cp1->bindOutput(first);

	// second = tmp;
	auto cp2 = fr->getFunc("Copy:long");
	cp2->bindInput(0, fr->getHandler(tmp));
	cp2->bindOutput(second);
	// result = second;
	auto cp3 = fr->getFunc("Copy:long");
	cp3->bindInput(0, fr->getHandler(second));
	cp3->bindOutput(result);

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
	//return *(long*)result->getData();
	
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

	fib_fce->bindInput(0, fr->getHandler(n));
	//fib_fce->bindOutput(output);
	


	RunStatus stat;
	for(long i = 0; i < 100; i++)
	{
		*(long*) n->getData() = i;
		(*fib_fce)(stat);
		std::cout << "Result: "<<*(long*) fib_fce->getOutput()->getData() << std::endl;
	}

	return 0;
}
