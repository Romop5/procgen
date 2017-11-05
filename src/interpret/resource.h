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

class Resource
{
	public:
	unsigned char* value;
	size_t type;
	std::shared_ptr<TypeRegister> tr;
	public:
	// TODO: remove this constructor
	Resource():tr(std::shared_ptr<TypeRegister>()),value(NULL), type(0){};
	Resource(std::shared_ptr<TypeRegister> typereg, 
		unsigned char* dt, size_t id): tr(typereg),value(dt),type(id){};
	~Resource()
	{
		if(value)
			delete[] value;
	}
	void* getData() {return value;}
	size_t getId() {return type;}
	
	//template<typename T>
	//T* operator*(){return dynamic_cast<T*>(value);}
	template<typename T>
	bool operator==(const T& val)
	{
		return ((*(T*) this->value) == val);
	}

	bool copy(const std::shared_ptr<Resource> src);

	std::string getName();
};


#endif
