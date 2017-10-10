#include <vector>
#include <iostream>
#include <string>
#include <map>
typedef void*(*constructor)();
class Factory
{
	public:
	std::map<std::string, constructor> classes;
	void* factor(std::string name) {return classes[name]();}
};

Factory gTypes;

#define DEF_OPER(OPERNAME,OPERATION)\
template<typename TYPE>\
class OPERNAME\
{\
	virtual void operator()()\
	{\
		TYPE a, b,c;\
		c = a OPERATION b;\
	}\
};
//template<typename TYPE>\
//void * OPERNAME##makeInstance() { return new OPERNAME<TYPE>();}\

#define REG(OPERNAME, TYPE)\
OPERNAME##makeInstance < TYPE >();
//void* OPERNAME##TYPE = &OPERNAME##makeInstance < TYPE >;

DEF_OPER(Add, +);
//REG(Add, int);
//
#define REGFAC(OPERNAME,TYPE)\
void* OPERNAME##TYPE () { return new OPERNAME<TYPE>();}\
struct str##OPERNAME##TYPE {\
	str##OPERNAME##TYPE() {gTypes.classes[#OPERNAME#TYPE] = &OPERNAME##TYPE;}\
} inst##OPERNAME;

REGFAC(Add,int);
//REGFAC(lol);


int main()
{
	for(auto &x: gTypes.classes)
	{
		std::cout << x.first << std::endl;
		x.second();
	}
}
