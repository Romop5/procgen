#include "resource.h"
#include "typereg.h"

bool Resource::copy(const std::shared_ptr<Resource> src)
{
	if(!value)
		return false;
	if(src->getId() != type)
		return false;

	size_t size = tr->getType(type)->getSize();
	memcpy(this->value,src->getData(),size);

	return true;
}

std::string Resource::getName()
{
	return tr->getTypeName(this->type);
}
