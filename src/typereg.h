#include <map>
#include <memory>
#include "typestring.h"
#include "resource.h"
class Type
{
	std::string typeName;
	size_t size;
	public:
	Type(){};
	Type(std::string name, size_t len) :size(len),typeName(name){};
	size_t getSize() {return this->size;}
	std::string getName() {return this->typeName;}
};
class TypeRegister
{
	using TypeId = size_t;
	public:
	const TypeId UNKWNOWN = 0;
	TypeRegister():highest(1){};
	template<typename T>
	bool add(const std::string& typeName)
	{
		TypeId id = highest++;
		types[id] = Type(typeName,sizeof(T));
		names[typeName] = id;
	}
	TypeId getTypeId(const std::string& name)
	{
		auto it = names.find(name);
		if(it != names.end())
			return it->second;
		return UNKWNOWN;
	}

	std::shared_ptr<Resource> sharedResource(TypeId type)
	{
		auto it = types.find(type);
		if(it != types.end())
		{
			void* data = new unsigned char[it->second.getSize()];
			return std::make_shared<Resource>(data,it->first);
		}
		return std::make_shared<Resource>();
	}
	
	private:
	TypeId highest;
	std::map<std::string,TypeId> names;
	std::map<TypeId,Type> types;
	
};
