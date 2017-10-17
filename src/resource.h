/*
 * Procedural generation library
 * xdobia11 - Roman Dobias
 * Purpose: defines an atomic data handling structure
 *
 */

#ifndef _RESOURCE_H
#define _RESOURCE_H
#include <cstddef>
class Resource
{
	public:
	void* value;
	size_t type;
	public:
	Resource():value(NULL), type(0){};
	Resource(void* dt, size_t id): value(dt),type(id){};
	void* getData() {return value;}
	size_t getId() {return type;}
	
	//template<typename T>
	//T* operator*(){return dynamic_cast<T*>(value);}
	template<typename T>
	bool operator==(const T& val)
	{
		return ((*(T*) this->value) == val);
	}
};


#endif
