#ifndef _TYPEDESC_H
#define _TYPEDESC_H
#include <stddef.h>
#include <stdalign.h>
#include <vector>
#include <memory>
//typedef int TypeId;
using TypeId = size_t;
enum descType {ATOMIC,COMPOSITE,COLLECTION,ABSTRACT,ANY};

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
			unsigned int size, std::vector<TypeId> compos,std::vector<std::string> items);
	virtual descType getType() { return COMPOSITE;};
	std::vector<TypeId> components;
	std::vector<std::string> componentsNames;

	unsigned int getOffset(unsigned int componentID);

	bool hasComponentWithName(const std::string name) const;
	size_t getComponentPositionByName(const std::string name) const;
    const std::string getComponentName(size_t componentID) const;
    TypeId getComponentTypeId(size_t componentID) const;
		
};
class CollectionType : public AbstractType
{
	public:
	CollectionType();
	virtual descType getType() { return COLLECTION;}
};

class AnyType: public AbstractType
{
	public:
	AnyType() {};
	virtual descType getType() { return ANY;}
};



#endif
