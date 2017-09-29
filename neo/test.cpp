#include "single.h"

int main()
{
	auto fct = ResourceFactory();
	auto r = fct.createResource("test");
	*(int*) r->value = 666;
	std::cout << "lol" << std::endl;
	fct.dump();


	auto box = Add();
	box.inputs.push_back(r);
	box.inputs.push_back(r);
	auto res = fct.createResource("result");
	box.output = res;
	box();

	std::cout << "Should be 2x666: " << *(int*) res->value << std::endl;

	fct.dump();
}
