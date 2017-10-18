#include "typereg.h"
#include <iostream>
#include <memory>
#include <string.h>


void* getComponent(std::shared_ptr<TypeRegister> tr,std::shared_ptr<Resource> composite, int id)
{
	auto type = tr->getType(composite->getId());
	if(type->getType() != COMPOSITE)
		return NULL;
	auto compo = std::dynamic_pointer_cast<CompositeType>(type);
	std::cout << "Size: " << type->getSize() << std::endl;
	return composite->getData() + compo->getOffset(id);
}

void copyComponent(std::shared_ptr<TypeRegister> tr,std::shared_ptr<Resource> composite,std::shared_ptr<Resource> dest, int id)
{
	auto type = tr->getType(composite->getId());
	if(type->getType() != COMPOSITE)
		return;
	auto compo = std::dynamic_pointer_cast<CompositeType>(type);
	std::cout << "Size: " << type->getSize() << std::endl;
	void* src = composite->getData() + compo->getOffset(id);

	delete[] dest->value;
	dest->value = new unsigned char[type->getSize()];
	memcpy(dest->value, src, type->getSize());
	

}

void setComponent(std::shared_ptr<TypeRegister> tr,std::shared_ptr<Resource> composite,std::shared_ptr<Resource> src, int id)
{
	auto compotype = tr->getType(composite->getId());
	auto smalltype = tr->getType(src->getId());
	if(compotype->getType() != COMPOSITE)
		return;
	auto compo = std::dynamic_pointer_cast<CompositeType>(compotype);

	void* dst = composite->getData() + compo->getOffset(id);

	memcpy(dst, src->getData(), smalltype->getSize());
	

}


