#ifndef _TYPE_H
#define  _TYPE_H

#include <map>
#include <memory>
#include "types.h"
#include "resource.h"
#include "typedesc.h"

/**
* @class TypeRegister 
* @brief Registration and instancing of user-defined types
*/
class TypeRegister: public std::enable_shared_from_this<TypeRegister> 
{
	//using TypeId = size_t;
	public:
	const TypeId UNKWNOWN = 0;
	TypeRegister():highest(0){
		add<char>("unknown");
		addAny();
		addCollection();
	};

    
/**
* @brief Add another name for existing type
*
* @param alias   The new name for type
* @param aliased The existing type
*
* @return \cFALSE if \calias name already exists
*/
	bool addAlias(const std::string& alias, const std::string& aliased)
	{
		// if aliased type exists and alias doesn't
		if(names.find(aliased) != names.end()
				&& names.find(alias) == names.end())
		{
			names[alias] = names[aliased]; 
			return true;
		}
		return false;
	}

/**
* @brief Register a new type paired with C++ type \cT
*
* @tparam T C++ type
* @param typeName Interpret-dependend name for type
*
* This function registers a new type \ctypeName with values of \cT
* @example add<int>("integer") registers a new variable integer
* which can store ints
* @return \cFALSE if type with the same name exists
*/
	template<typename T>
	bool add(const std::string& typeName)
	{
		TypeId id = highest++;
		types[id] = std::make_shared<AtomicType>(sizeof(T));
		names[typeName] = id;
	}

/**
* @brief Register a new structured type from other types
*
* @param typeName The name for structured type in interpret domain
* @param types The vector of \cTypeId of other types
*
* @return \cFALSE if type with the same name exists
* @see TypeRegister::addCompositeWithNames
*/
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
/**
* @brief Register a new structured type from other named types 
*
* @param typeName   The name for structured type in interpret domain
* @param types      The vector of \cTypeId of other types
* @param itemNames  The names of member subtypes
*
* @return \cFALSE if type with the same name exists
* @see TypeRegister::addComposite
*/
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

/**
* @brief Internally register collection type
*/
	bool addCollection()
	{
		TypeId id = highest++;
		types[id] = std::make_shared<CollectionType>();
		names["collection"] = id;
	}
/**
* @brief Internally register 'any' type
*/
	bool addAny()
	{
		TypeId id = highest++;
		types[id] = std::make_shared<AnyType>();
		names["any"] = id;
	}

/**
* @brief Get \cTypeId by type name
*
* @param name
*
* @return \cUNKNOWN if name doesn't exist
*/
	TypeId getTypeId(const std::string& name)
	{
		auto it = names.find(name);
		if(it != names.end())
			return it->second;
		return UNKWNOWN;
	}
/**
* @brief Get typename for given id
*
* @param id
*
* @return "typeNotFound" if \cid doesn't exist  
*/
	std::string getTypeName(const TypeId id)
	{
		for(auto &x: this->names)
		{
			if(x.second == id)
				return x.first;
		}
		return "typeNotFound";
	}
/**
* @brief Create an instance of Resource of registered type
*
* @param type
*
* @return \cnullptr if given TypeId is not valid
*/
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
					return std::make_shared<CollectionResource>(shared_from_this(), it->first);
					}
					break;
				case COMPOSITE:
					{
						auto compo = std::dynamic_pointer_cast<CompositeType>(it->second);
						std::map<size_t,std::shared_ptr<Resource>> map;
						for(int i = 0; i < compo->getComponentCount() ;i++)
						{
							auto res = this->sharedResource(compo->getComponentTypeId(i));
							map[i] = res;
						}
						return std::make_shared<CompositeResource>(shared_from_this(),type,map);
					}
				case ANY:
					{
						return std::make_shared<AnyResource>(shared_from_this());
					}
					break;
				default:
					// TODO exception
					break;
			}
		}
		return nullptr;
	}
	
    
/**
* @brief Create an instance of type
*
* @param name
*
* @return \cnullptr if name doesn't exist
*/
	std::shared_ptr<Resource> sharedResource(std::string name)
	{
		auto id = this->getTypeId(name);
        if(hasTypeWithID(id))
            return this->sharedResource(id);
        return nullptr;
	}
	

/**
* @brief Get TypeDescription for TypeId 
*
* @param id
*
* @return TypeDescription of \cUNKWNOWN if id is invalid
*/
	std::shared_ptr<AbstractType>& getType(TypeId id)
	{
		auto it = types.find(id);
		if(it != types.end())
		{
			return it->second;
		}
		return types[UNKWNOWN];	
	}


/**
* @brief Is id a valid TypeId ?
*
* @param id
*
* @return FALSE if not
*/
    bool hasTypeWithID(TypeId id) const
    {
        if(id == 0)
            return false;
        if(id >= highest)
            return false;
        return true;
    }
    
/**
* @brief Is name registered
*
* @param name
*
* @return FALSE if not 
*/
    bool hasType(const std::string& name)
    {
        auto it = getTypeId(name);
        return hasTypeWithID(it);
    }

	private:
	TypeId highest;
	std::map<std::string,TypeId> names;
	std::map<TypeId,std::shared_ptr<AbstractType>> types;
	
	
};
#endif
