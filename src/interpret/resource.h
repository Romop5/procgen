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

class Resource 
{
	
	protected:
	ResourceType resourceType;
	size_t baseType;
	std::shared_ptr<TypeRegister> tr;
	public:
	//Resource():tr(std::shared_ptr<TypeRegister>()),value(NULL), type(0){};
	//Resource(std::shared_ptr<TypeRegister> typereg, 
	//	unsigned char* dt, size_t id): tr(typereg),value(dt),type(id){};
	
	// Virtual destructor
	virtual ~Resource() {}

	// Virtual data accessor
	virtual void* getData() const = 0;

	// Get base type id (registered in TypeRegister)
	virtual size_t getBaseId() {return baseType;}
	
	ResourceType getResourceType() {return resourceType;}
	// Get base type name
	std::string getName();

	/*template<typename T>
	bool operator==(const T& val)
	{
		return ((*(T*) this->value) == val);
	}
	*/
	virtual bool copy(const std::shared_ptr<Resource> src) = 0;
    
    virtual json to_json() const { return json("UnkResource"); }

    ResourceType    getResourceType() const { return this->resourceType; }

};


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
	
	//template<typename T>
	//T* operator*(){return dynamic_cast<T*>(value);}
	/*
	template<typename T>
	bool operator==(const T& val)
	{
		return ((*(T*) this->value) == val);
	}

	bool copy(const std::shared_ptr<Resource> src);
	*/

	virtual bool copy(const std::shared_ptr<Resource> src) override;
    virtual json to_json() const override;

};

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
	// TODO
	virtual bool copy(const std::shared_ptr<Resource> src) override;
	virtual void* getData() const override {};
    virtual json to_json() const override;
    const std::string getComponentName(size_t index) const;
    size_t getComponentPosition(const std::string name) const;
    TypeId getComponentType(size_t index) const;
};

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
	virtual size_t getBaseId();
	std::shared_ptr<Resource> getContent() const { return content; }	
	virtual bool copy(const std::shared_ptr<Resource> src) override;
	virtual json to_json() const override;

};

#endif
