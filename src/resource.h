#ifndef _RESOURCE_H
#define _RESOURCE_H
/*class Resource
{
	public:
	unsigned int type;
	void* value;	
};
*/
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
};


#endif
