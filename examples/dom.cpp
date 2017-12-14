#include "derivation.h"
#include "appender.h"

class AlwaysTrue : public Function
{
	public:
	virtual bool operator()(RunStatus& rs)
	{
		*(bool*) getOutput()->getData() = true;
		return false;
	}
};

class ruleInt: public Function
{
	Derivation* der;
	public:
	ruleInt(Derivation* der)
	{
		this->der = der;
	}
	virtual bool operator()(RunStatus& rs)
	{	
		std::cout << "Producing int and float \n";
		// Producing int 
		this->der->appendNextSymbol(_getInput(0)->getOutput());
		// Producing float 
		this->der->appendNextSymbol(der->tr->sharedResource("float"));
		return false;
	}
};


std::string convertSymbolsToString(TypeId typeInt, const std::vector<std::shared_ptr<Resource>> symbols)
{
	std::string result = "";
	for(auto &x: symbols)
	{
		if(x->getBaseId() == typeInt)
			result += "A";
		else
			result += "B";
	}
	return result;
}

int main(int argc, char **argv)
{
	std::cerr << "Produces Nth generation of algae string" << std::endl;
	if(argc < 2)
	{
		std::cout << "Format: <iteration-count>" << std::endl;
	}

	srandom(time(NULL));
	
	auto tr = std::make_shared<TypeRegister>();
	auto fr = std::make_shared<FunctionReg>(tr);
	
	registerStandardTypes(tr.get());
	registerStandardFunctions(fr.get());

	auto derivation = std::make_shared<Derivation>(tr,fr);


	/* L-sys grammar */

	// int -> int float 
	// float -> int
	
	auto intValue = tr->sharedResource("int");

	std::vector<std::shared_ptr<Resource>> symbols = {intValue};

	derivation->setStartSymbols(symbols);

	// Add rule
	
	derivation->addRule(tr->getTypeId("int"), std::make_shared<AlwaysTrue>(), std::make_shared<ruleInt>(derivation.get()));

	derivation->generate(atoi(argv[1]));
	
	//derivation._debug();
	//
	auto syms= derivation->getCurrentSymbolList();

	auto str = convertSymbolsToString(tr->getTypeId("int"), syms);

	std::cout << str << std::endl;


	return 0;
}
