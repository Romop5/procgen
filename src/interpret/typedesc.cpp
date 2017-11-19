#include "typedesc.h"
#include "typereg.h"

unsigned int AbstractType::getAlignedSize()
{
	size_t alignment = alignof(max_align_t);
	size_t rest = this->size % alignment;
	if(rest == 0)
		return this->size;
	return this->size-rest+alignment;
}
CompositeType::CompositeType(std::shared_ptr<TypeRegister> tr,unsigned int size, std::vector<TypeId> compos)
{
	this->tr = tr;
	this->size = size;
	this->components = compos;
}
unsigned int CompositeType::getOffset(unsigned int componentID)
{
	unsigned int sum = 0;
	if(components.size() <= componentID)
		return -1;
	for(int i = 0; i < componentID;i++)
	{
		auto ut = tr->getType(components[i]);
		sum += ut->getAlignedSize();
	}
	return sum;
}


CollectionType::CollectionType(std::shared_ptr<TypeRegister>,
			TypeId baseType)
{
	this->baseType = baseType;
}
