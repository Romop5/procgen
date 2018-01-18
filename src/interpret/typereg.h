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
        // Fail on invalid type
        for(auto type: types)
        {
            if(hasTypeWithID(type) == false)
                return false;
        }

		TypeId id = highest++;
		unsigned int size = 0;
		for(auto &x: types)
			size += this->getType(x)->getAlignedSize();

		std::vector<std::string> componentNames;
		this->types[id] = std::make_shared<CompositeType>(shared_from_this(),size,types,componentNames);
		names[typeName] = id;

        return true;

	}
	bool addCompositeWithNames(const std::string& typeName,std::vector<TypeId> types,std::vector<std::string> itemNames)
	{
        // Fail on invalid type
        for(auto type: types)
        {
            if(hasTypeWithID(type) == false)
                return false;
        }
        // Increment type count
		TypeId id = highest++;
        // TODO
        // Compute the size of structure
		unsigned int size = 0;
		for(auto &x: types)
			size += this->getType(x)->getAlignedSize();

		// TODO: refactor items and their names
		this->types[id] = std::make_shared<CompositeType>(shared_from_this(),size,types,itemNames);
		names[typeName] = id;

	}

	bool addCollection(const std::string& typeName,TypeId baseType)
	{
		TypeId id = highest++;
		types[id] = std::make_shared<CollectionType>(shared_from_this(),baseType);
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
			switch(it->second->getType())
			{
				case ATOMIC:
					{
					unsigned char* data = new unsigned char[it->second->getSize()];
					return std::make_shared<AtomicResource>(shared_from_this(),data,it->first);
					}
					break;
				case COLLECTION:
					{
					TypeId baseType = (std::dynamic_pointer_cast<CollectionType>(it->second))->getBaseType();
					return std::make_shared<CollectionResource>(shared_from_this(),baseType, it->first);
					}
					break;
				case COMPOSITE:
					{
						auto compo = std::dynamic_pointer_cast<CompositeType>(it->second);
						std::map<size_t,std::shared_ptr<Resource>> map;
						for(int i = 0; i < compo->components.size();i++)
						{
							auto res = this->sharedResource(compo->components[i]);
							map[i] = res;
						}
						return std::make_shared<CompositeResource>(shared_from_this(),type,map);
					}
				default:
					// TODO exception
					break;
			}
		}
		return nullptr;
	}
	
	std::shared_ptr<Resource> sharedResource(std::string name)
	{
		auto id = this->getTypeId(name);
        if(hasTypeWithID(id))
            return this->sharedResource(id);
        return nullptr;
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

    bool hasTypeWithID(TypeId id) const
    {
        if(id == 0)
            return false;
        if(id >= highest)
            return false;
        return true;
    }
	private:
	TypeId highest;
	std::map<std::string,TypeId> names;
	std::map<TypeId,std::shared_ptr<AbstractType>> types;
	
	
};
#endif
