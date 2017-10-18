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
	unsigned char* value;
	size_t type;
	public:
	Resource():value(NULL), type(0){};
	Resource(unsigned char* dt, size_t id): value(dt),type(id){};
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
};


#endif
