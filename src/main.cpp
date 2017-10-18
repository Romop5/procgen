#include "typereg.h"
#include "compositeutils.h"
#include <iostream>

int main()
{
	auto tr = std::make_shared<TypeRegister>();
	auto i = tr->add<int>("int");
	auto c = tr->add<char>("char");
	std::vector<TypeId> types {i,c};
	auto ic = tr->addComposite("ic", types);

	auto intinst= tr->sharedResource("char");
	auto intinstB =  tr->sharedResource("char");
	*(int*) intinst->getData() = 'L';
	auto icinst = tr->sharedResource(ic);
	setComponent(tr, icinst, intinst,1);

	std::cout <<  getComponent(tr,icinst,0) << std::endl;
	std::cout <<  getComponent(tr,icinst,1) << std::endl;
	types.push_back(ic);
	auto lol= tr->addComposite("lol", types);

	auto lolinst= tr->sharedResource(lol);
	std::cout <<  getComponent(tr,lolinst,0) << std::endl;
	std::cout <<  getComponent(tr,lolinst,1) << std::endl;
	std::cout <<  getComponent(tr,lolinst,2) << std::endl;

	setComponent(tr, lolinst, icinst,2);
	
	copyComponent(tr, lolinst, icinst,2);
	copyComponent(tr, icinst, intinstB,1);

	std::cout << "Value: " << *(char*) intinstB->getData() << std::endl;
	return 0;
}
