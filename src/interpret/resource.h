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

class TypeRegister;

enum class ResourceType {ABSTRACT, ATOMIC, COMPOSITE, COLLECTION};

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
	virtual void* getData() = 0;

	// Get base type id (registered in TypeRegister)
	size_t getBaseId() {return baseType;}
	
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
	virtual void* getData() {return value;}
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

	virtual bool copy(const std::shared_ptr<Resource> src);

};

class CompositeResource : public Resource
{
	private:
		std::map<size_t, std::shared_ptr<Resource>> components;
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
	virtual bool copy(const std::shared_ptr<Resource> src);
	virtual void* getData(){};

};

class CollectionResource : public Resource
{
	private:
		size_t arrayType;
		std::vector<std::shared_ptr<Resource>> collection;
	public:
	CollectionResource(std::shared_ptr<TypeRegister> typereg, size_t baseType, size_t arrayType)
	{
		this->tr = typereg;
		this->baseType = baseType;
		this->arrayType = arrayType;
		this->resourceType = ResourceType::COLLECTION;
	}

	~CollectionResource()
	{
	}

	void append(std::shared_ptr<Resource> item);
	void remove(size_t index);
	std::shared_ptr<Resource> at(size_t index);
	size_t length() {return this->collection.size();}
	size_t getArrayType() {return this->arrayType;}

	virtual void* getData(){};
	// TODO
	virtual bool copy(const std::shared_ptr<Resource> src);
};

#endif
