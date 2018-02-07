#include "resource.h"
#include "typereg.h"
#include <sstream>

bool AtomicResource::copy(const std::shared_ptr<Resource> src)
{
	if(!value)
		return false;
	if(src->getBaseId() != baseType)
		return false;

	size_t size = tr->getType(baseType)->getSize();
	memcpy(this->value,src->getData(),size);

	return true;
}

json11::Json AtomicResource::to_json() const
{
    std::string nameOfType = tr->getTypeName(baseType);
    if(nameOfType == "float")
        return json11::Json( *(float*) this->getData()); 

    if(nameOfType == "int")
        return json11::Json( *(int*) this->getData()); 

    if(nameOfType == "bool")
        return json11::Json( *(bool*) this->getData()); 

    return json11::Json("unkAtomicValue");
}

std::string Resource::getName()
{
	return tr->getTypeName(this->baseType);
}

bool CompositeResource::copy(const std::shared_ptr<Resource> src)
{
	auto srcComposite = std::dynamic_pointer_cast<CompositeResource>(src);
	for(size_t i = 0; i < this->components.size(); i++)
	{
		this->getComponent(i)->copy(srcComposite->getComponent(i));
	}
}

void CollectionResource::append(std::shared_ptr<Resource> item)
{
	if(item->getBaseId() == this->getBaseId())
	{
		auto newElement = this->tr->sharedResource(item->getBaseId());
		newElement->copy(item);
		this->collection.push_back(newElement);
	}
}

void CollectionResource::remove(size_t index)
{
	this->collection.erase(this->collection.begin()+index);
}


std::shared_ptr<Resource> CollectionResource::at(size_t index)
{
	return this->collection[index];
}

bool CollectionResource::copy(const std::shared_ptr<Resource> src)
{

	auto srcCollection = std::dynamic_pointer_cast<CollectionResource>(src);
	this->collection.clear();
	for(size_t i = 0; i < srcCollection->length(); i++)
	{
		auto res = this->tr->sharedResource(srcCollection->at(i)->getBaseId());
		res->copy(srcCollection->at(i));
		this->collection.push_back(res);
	}
}

json11::Json CompositeResource::to_json() const
{
    json11::Json::object object;
	for(size_t i = 0; i < this->components.size(); i++)
	{
		object[getComponentName(i)] = (this->components.at(i)->to_json());
    }
    return json11::Json(object);
}

std::shared_ptr<CompositeType>  CompositeResource::getCompositeTypeDescription() const
{
    auto typeDesc = tr->getType(baseType);    
    return std::dynamic_pointer_cast<CompositeType>(typeDesc);
}

const std::string CompositeResource::getComponentName(size_t index) const
{
    auto compositeDescription = this->getCompositeTypeDescription();
    return compositeDescription->getComponentName(index);
}

size_t CompositeResource::getComponentPosition(const std::string name) const
{
    auto compositeDescription = this->getCompositeTypeDescription();
    return compositeDescription->getComponentPositionByName(name);
}

TypeId CompositeResource::getComponentType(size_t index) const
{
    auto compositeDescription = this->getCompositeTypeDescription();
    return compositeDescription->getComponentTypeId(index);
}
