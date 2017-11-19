#include "resource.h"
#include "typereg.h"

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
