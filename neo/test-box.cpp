#include "single.h"

int main()
{
	auto fct = ResourceFactory();

	// create box for a+a
	auto a = fct.createResource("a");
	auto box = Add();
	box.inputs.push_back(a);
	box.inputs.push_back(a);
	auto res = fct.createResource("result");
	box.output = res;

	*(int*) a->value = 666;
	box();

	std::cout << "Should be 2x666: " << *(int*) res->value << std::endl;

	fct.dump();
	// create box for (a+a)*10
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


	std::cout << *(int*) mul.output->value << std::endl;
	fct.dump();
}
