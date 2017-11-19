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
//#include "typereg.h"

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

class CollectionResouce : public Resource
{
	private:
		size_t arrayType;
	public:
	std::vector<std::shared_ptr<Resource>> collection;
	public:
	CollectionResouce(std::shared_ptr<TypeRegister> typereg, size_t baseType, size_t arrayType)
	{
		this->tr = typereg;
		this->baseType = baseType;
		this->arrayType = arrayType;
	}

	~CollectionResouce()
	{
	}

	void append(std::shared_ptr<Resource> item)
	{
		if(item->getBaseId() == this->getBaseId())
		{
			auto newElement = this->tr->sharedResource(item->getBaseId());
			newElement.copy(item);
			this->collection.push_back(newElement);
		}
	}

};

#endif
