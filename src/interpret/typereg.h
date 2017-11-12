#ifndef _TYPE_H
#define  _TYPE_H

#include <map>
#include <memory>
#include "types.h"
#include "resource.h"
#include "typedesc.h"

class TypeRegister: public std::enable_shared_from_this<TypeRegister> 
{
	//using TypeId = size_t;
	public:
	const TypeId UNKWNOWN = 0;
	TypeRegister():highest(0){
		add<char>("unknown");
	};
	template<typename T>
	bool add(const std::string& typeName)
	{
		TypeId id = highest++;
		types[id] = std::make_shared<AtomicType>(sizeof(T));
		names[typeName] = id;
	}
	bool addComposite(const std::string& typeName,std::vector<TypeId> types)
	{
		TypeId id = highest++;
		unsigned int size = 0;
		for(auto &x: types)
			size += this->getType(x)->getAlignedSize();

		this->types[id] = std::make_shared<CompositeType>(shared_from_this(),size,types);
		names[typeName] = id;

	}
	TypeId getTypeId(const std::string& name)
	{
		auto it = names.find(name);
		if(it != names.end())
			return it->second;
		return UNKWNOWN;
	}
	std::string getTypeName(const TypeId id)
	{
		for(auto &x: this->names)
		{
			if(x.second == id)
				return x.first;
		}
		return "typeNotFound";
	}
	std::shared_ptr<Resource> sharedResource(TypeId type)
	{
		auto it = types.find(type);
		if(it != types.end())
		{
			unsigned char* data = new unsigned char[it->second->getSize()];
			return std::make_shared<Resource>(shared_from_this(),data,it->first);
		}
		return std::make_shared<Resource>();
	}
	
	std::shared_ptr<Resource> sharedResource(std::string name)
	{
		auto id = this->getTypeId(name);
		return this->sharedResource(id);
	}
	

	std::shared_ptr<AbstractType>& getType(TypeId id)
	{
		auto it = types.find(id);
		if(it != types.end())
		{
			return it->second;
		}
		return types[UNKWNOWN];	
	}
	private:
	TypeId highest;
	std::map<std::string,TypeId> names;
	std::map<TypeId,std::shared_ptr<AbstractType>> types;
	
	
};
#endif
