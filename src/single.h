#include "function.h"
#include "statement.h"

class ResourceFactory
{
	private:
	std::map<std::string,std::shared_ptr<Resource>> resources;
	public:
	std::shared_ptr<Resource> createResource(std::string name)
	{
		resources[name] = std::make_shared<Resource>();
		// TODO: alloc mem according to type
		resources[name]->value = new int();
		return resources[name];
	}

	void dump()
	{
		std::cout << "Dumping resources:\n";
		for(auto x: resources)
			std::cout << x.first << " - " << *(int*) x.second->value << std::endl;
	}

};
