#ifndef _TYPEDESC_H
#define _TYPEDESC_H
#include <stddef.h>
#include <stdalign.h>
#include <vector>
#include <memory>
//typedef int TypeId;
using TypeId = size_t;
enum descType {ATOMIC,COMPOSITE,COLLECTION,ABSTRACT};

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

	bool hasComponentWithName(const std::string name);
	size_t getComponentPositionByName(const std::string name);
    const std::string getComponentName(size_t componentID);
		
};
class CollectionType : public AbstractType
{
	std::shared_ptr<TypeRegister> tr;
	TypeId baseType;
	public:
	CollectionType(std::shared_ptr<TypeRegister>,
			TypeId baseType);
	virtual descType getType() { return COLLECTION;}
	TypeId getBaseType() { return baseType;}
};

#endif
