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
