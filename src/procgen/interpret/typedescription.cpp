#include <iostream>
#include <procgen/interpret/typedescription.h>
#include <procgen/interpret/typeregister.h>
#include <procgen/utils/logger.h>

unsigned int AbstractType::getAlignedSize()
{
    /*    size_t alignment = alignof(max_align_t);
    size_t rest = this->size % alignment;
    if (rest == 0)
        return this->size;
    return this->size - rest + alignment;
	*/
    return 0;
}
CompositeType::CompositeType(std::weak_ptr<TypeRegister> tr, unsigned int size, std::vector<TypeId> compos, std::vector<std::string> items)
{
    this->tr = tr;
    this->size = size;
    this->components = compos;
    this->componentsNames = items;
}

CollectionType::CollectionType()
{
}

bool CompositeType::hasComponentWithName(const std::string name) const
{
    for (auto& x : this->componentsNames)
        if (x == name)
            return true;
    return false;
}

size_t CompositeType::getComponentPositionByName(const std::string name) const
{
    LOG_DEBUG("Looking for %s in: \n", name.c_str());

    for (auto& x : this->componentsNames) {
        LOG_DEBUG("'%s'\n", x.c_str());
        static_cast<void>(x);
    }

    for (size_t x = 0; x < this->componentsNames.size(); x++)
        if (this->componentsNames[x] == name)
            return x;
    return COMPOSITE_COMPONENT_NOT_FOUND;
}

const std::string CompositeType::getComponentName(size_t componentID) const
{
    return this->componentsNames[componentID];
}

TypeId CompositeType::getComponentTypeId(size_t componentID) const
{
    if (componentID >= this->components.size())
        return 0;
    return this->components[componentID];
}
