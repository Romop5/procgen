#include "typereg.h"
#include <iostream>
#include <memory>
#include <string.h>
#include "function.h"

void copyComponent(std::shared_ptr<TypeRegister> tr,std::shared_ptr<Resource> composite,std::shared_ptr<Resource> dest, int id);
void setComponent(std::shared_ptr<TypeRegister> tr,std::shared_ptr<Resource> composite,std::shared_ptr<Resource> src, int id);
// First input = resource
// Second input = index
// Output = component of input with index
// output= composite[index] 
class AccessComposite: public Function
{
	private:
		std::shared_ptr<TypeRegister> tr;
	public:
	AccessComposite(std::shared_ptr<TypeRegister> tr)
	{
		this->tr = tr;
	}
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;

		copyComponent(this->tr,this->_getInput(0)->getOutput(),this->getOutput(),
			       *(size_t*) this->_getInput(1)->getOutput()->getData());
		return false;
	}

};

// First input = resource
// Second input = index
// Output = component of input with index
// output[index] = component
class SetComposite: public Function
{
	private:
		std::shared_ptr<TypeRegister> tr;
	public:
	SetComposite(std::shared_ptr<TypeRegister> tr)
	{	
		this->tr = tr;
	}
	virtual bool operator()(RunStatus& stat)
	{
		if(_doInputs(stat)) return true;
		setComponent(this->tr,this->getOutput(),this->_getInput(0)->getOutput(),
			       *(unsigned char*) this->_getInput(1)->getOutput()->getData());
		return false;
	}

};



void* getComponent(std::shared_ptr<TypeRegister> tr,std::shared_ptr<Resource> composite, int id)
{
	auto type = tr->getType(composite->getId());
	if(type->getType() != COMPOSITE)
		return NULL;
	auto compo = std::dynamic_pointer_cast<CompositeType>(type);
	std::cout << "Size: " << type->getSize() << std::endl;
	return (void*) ((unsigned char*) composite->getData() + compo->getOffset(id));
}

void copyComponent(std::shared_ptr<TypeRegister> tr,std::shared_ptr<Resource> composite,std::shared_ptr<Resource> dest, int id)
{
	auto type = tr->getType(composite->getId());
	if(type->getType() != COMPOSITE)
		return;
	auto compo = std::dynamic_pointer_cast<CompositeType>(type);
	std::cout << "Size: " << type->getSize() << std::endl;
	void* src =  (void*) ((unsigned char*) composite->getData() + compo->getOffset(id));


	delete[] (unsigned char*) dest->value;
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

	void* dst = (void*) ((unsigned char*) composite->getData() + compo->getOffset(id));

	memcpy(dst, src->getData(), smalltype->getSize());
	

}


