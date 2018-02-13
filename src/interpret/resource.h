/*
 * Procedural generation library
 * xdobia11 - Roman Dobias
 * Purpose: defines an atomic data handling structure
 *
 */

#ifndef _RESOURCE_H
#define _RESOURCE_H
#include <cstddef>
#include <string.h>
#include <memory>
#include <vector>
#include <map>

#include "json.hpp"

using TypeId = size_t;
class CompositeType;
class TypeRegister;

enum class ResourceType {ABSTRACT, ATOMIC, COMPOSITE, COLLECTION,ANY};

using json = nlohmann::json;

/**
* @class Resource
* @brief Abstract class for type instances
*
* Each registered type has a value object - Resource. Primitive types
* have atomic resources which only contain TypeId and array of bytes.
* Structured types are composed of other types (structured or atomic).
* Structured types are either composite (structs) or arrays (collection).
*
* Resource and its derived classes follow composite pattern.
*/
class Resource 
{
	
	protected:
	ResourceType resourceType;
	size_t baseType;
	std::shared_ptr<TypeRegister> tr;
	public:
	
	virtual ~Resource() {}

	virtual void* getData() const = 0;

	virtual size_t getBaseId() {return baseType;}
	
	ResourceType getResourceType() {return resourceType;}

	std::string getTypeName();

	virtual bool copy(const std::shared_ptr<Resource> src) = 0;
    
    virtual json to_json() const { return json("UnkResource"); }

    ResourceType    getResourceType() const { return this->resourceType; }

};


/**
* @class AtomicResource
* @brief Exact-type value
*
* Serves as leaf node of Resource tree hierarchy. Always contains the raw
* value of type. Represents type such as int, float, bool.
*/
class AtomicResource : public Resource
{
	private:
	unsigned char* value;
	public:
	AtomicResource(std::shared_ptr<TypeRegister> typereg, 
		unsigned char* dt, size_t id) 
	{
		this->tr = typereg;
		this->value = dt;
		this->baseType = id;
	}
	virtual ~AtomicResource()
	{
		if(value)
			delete[] value;
	}
	virtual void* getData() const override {return value;}
	void setData(void* dt) { this->value = (unsigned char*) dt;}

	virtual bool copy(const std::shared_ptr<Resource> src) override;
    virtual json to_json() const override;

};

/**
* @class CompositeResource
* @brief Structured resource type
*
* Builds up a tree from other resources, notable atomic resources.
* Each subresource is a member of structure and has a name. 
*/
class CompositeResource : public Resource
{
	private:
		std::map<size_t, std::shared_ptr<Resource>> components;
        std::shared_ptr<CompositeType>  getCompositeTypeDescription() const;
	public:
	CompositeResource(std::shared_ptr<TypeRegister> typereg,size_t base, std::map<size_t, std::shared_ptr<Resource>> data)
	{
		this->baseType = base;
		this->components = data;
		this->tr = typereg;
		this->resourceType = ResourceType::COMPOSITE;
	}

	std::shared_ptr<Resource> getComponent(size_t index) { return components[index]; }
	virtual bool copy(const std::shared_ptr<Resource> src) override;
	virtual void* getData() const override {};
    virtual json to_json() const override;
    const std::string getComponentName(size_t index) const;
    size_t getComponentPosition(const std::string name) const;
    TypeId getComponentType(size_t index) const;
};

/**
* @class CollectionResource
* @brief Collection of arbitrary types
*/
class CollectionResource : public Resource
{
	private:
		size_t arrayType;
		std::vector<std::shared_ptr<Resource>> collection;
	public:
	CollectionResource(std::shared_ptr<TypeRegister> typereg, size_t arrayType)
	{
		this->tr = typereg;
		this->baseType = arrayType;
		this->resourceType = ResourceType::COLLECTION;
	}

	~CollectionResource()
	{
	}

	void append(std::shared_ptr<Resource> item);
	void remove(size_t index);
	std::shared_ptr<Resource> at(size_t index);
	size_t length() {return this->collection.size();}
	virtual void* getData() const override {};
	// TODO
	virtual bool copy(const std::shared_ptr<Resource> src) override;
	    virtual json to_json() const override;
};

/**
* @class AnyResource
* @brief Serves as container for any resource
*
* This type of resource is neccessary to store arbitrary resource in resource.
* The feature is used to implement passing arbitrary type which can be
* verified during run-time.
*/
class AnyResource : public Resource
{
	private:
	std::shared_ptr<Resource> content;
	public:
	AnyResource(std::shared_ptr<TypeRegister> typereg)
	{
		this->tr = typereg;
		this->resourceType = ResourceType::ANY;
	}
	virtual ~AnyResource() {}
	virtual void* getData() const override 
	{
		if(content)
			return content->getData();
		return nullptr;
	}
	virtual size_t getBaseId() override;
	std::shared_ptr<Resource> getContent() const { return content; }	
	virtual bool copy(const std::shared_ptr<Resource> src) override;
	virtual json to_json() const override;

};

#endif
