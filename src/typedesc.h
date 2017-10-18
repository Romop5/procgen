#ifndef _TYPEDESC_H
#define _TYPEDESC_H
#include <stddef.h>
#include <stdalign.h>
#include <vector>
#include <memory>
//typedef int TypeId;
using TypeId = size_t;
enum descType {ATOMIC,COMPOSITE,ABSTRACT};

class	TypeRegister;

class	AbstractType 
{
	protected:
	unsigned int size;
	public:
	// Virtual destructor
	AbstractType():size(0){}
	virtual ~AbstractType() {}
	virtual descType getType() {return ABSTRACT;}
	unsigned int getSize() {return size;}
	unsigned int getAlignedSize();
};

class AtomicType : public AbstractType
{
	public:
	AtomicType(unsigned int size)
	{
		this->size = size;
	}
	virtual descType getType() { return ATOMIC;};
	
};
class CompositeType : public AbstractType
{
	std::shared_ptr<TypeRegister> tr;
	public:
	CompositeType(std::shared_ptr<TypeRegister>,
			unsigned int size, std::vector<TypeId> compos);
	virtual descType getType() { return COMPOSITE;};
	std::vector<TypeId> components;
	unsigned int getOffset(unsigned int componentID);
		
};

#endif
