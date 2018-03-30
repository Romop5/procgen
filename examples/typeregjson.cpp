#include <procgen/procgen.h>
#include <procgen/interpret/std.h>

using namespace  nlohmann;
int main()
{
	auto tr = std::make_shared<TypeRegister>();
	registerStandardTypes(tr.get());

	auto action= tr->addCompositeWithNames("vec", {tr->getTypeId("int"), tr->getTypeId("float")}, {"number", "size"});
	assert(action == true);

	
	//json test = "5"_json;
	json test = {
		{ "_type", "vec"},
		{ "number", 5},
		{ "size", 1.0}
	};
	std::cout << "Before: " << test << std::endl;
	for(auto &x: test)
	{
		std::cout << "For: " << x << std::endl;
	}
	auto result = tr->createResourceFromJson(test);
	assert(result != nullptr);
	if(result == nullptr)
		return 1;
	std::cout << "Result: " << result->to_json() << std::endl;
}
