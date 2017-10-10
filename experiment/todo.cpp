int main()
{
	// let's say we want to work with float
	//
	// float a,b,c;
	// c = a * b;
	//
	// now we want to work with int
	// d = e*f;
	// }
	
	// get float resources
	// createResource(getType("float"))
	// createResource(getType("float[3]"))
	//
	// 1. Transform type to ID e.g. getId<float>,
	// 2. create string to ID mapping e.g. getId("float"), for aliases
	// 3. create source allocation according to type
	//
	// Tutorial:
	// 1. create type id for new type
	// 2. for given type id, register new allocator 
	// 3. THAT's IT, enjoy new type
	// }
	//
	// COMPOSITE TYPES
	// E.G
	// vec3 either as a float[3] built-in or
	// vec3: array of float;
	// =>
	// class Array:public Resource
	// {
	// 	std::vector<Resource> data;
	// 	public:
	// 	Array(typeId type,size_t count)
	// 	{
	// 		for(size_t i = 0; i < count;i++)
	// 			data.push_back(allocate(type));
	// 	}
	// 	std::shared<Resource> getElement(size_t id) {return data[id]);
	// }
