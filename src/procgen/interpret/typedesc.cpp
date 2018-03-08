#include <procgen/interpret/typedesc.h>
#include <procgen/interpret/typereg.h>
#include <procgen/utils/logger.h>
#include <iostream>

unsigned int AbstractType::getAlignedSize()
{
	size_t alignment = alignof(max_align_t);
	size_t rest = this->size % alignment;
	if(rest == 0)
		return this->size;
	return this->size-rest+alignment;
}
CompositeType::CompositeType(std::shared_ptr<TypeRegister> tr,unsigned int size, std::vector<TypeId> compos,std::vector<std::string> items)
{
	this->tr = tr;
	this->size = size;
	this->components = compos;
	this->componentsNames = items;
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


CollectionType::CollectionType()
{
}



bool CompositeType::hasComponentWithName(const std::string name) const
{
	for(auto &x: this->componentsNames)
		if(x == name)
			return true;
	return false;
}

size_t CompositeType::getComponentPositionByName(const std::string name) const
{
    LOG_DEBUG("Looking for %s in: \n",name.c_str());
    for(auto &x: this->componentsNames)
    {
        LOG_DEBUG("'%s'\n",x.c_str());
    }

	for(size_t x = 0 ; x < this->componentsNames.size(); x++)
		if(this->componentsNames[x] == name)
			return x;
	return -1;

}

const std::string CompositeType::getComponentName(size_t componentID) const
{
   return this->componentsNames[componentID]; 
}

TypeId CompositeType::getComponentTypeId(size_t componentID) const
{
    if(componentID >= this->components.size())
        return 0;
    return this->components[componentID];
}
