#include "derivation.h"

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

class ruleFloat: public Function
{
	Derivation* der;
	public:
	ruleFloat(Derivation* der)
	{
		this->der = der;
	}
	virtual bool operator()(RunStatus& rs)
	{	
		// Producing A
		std::cout << "Producing int from float \n";
		this->der->appendNextSymbol(der->tr->sharedResource("int"));
		return false;
	}
};

int main(int argc, char **argv)
{
	srandom(time(NULL));
	
	auto tr = std::make_shared<TypeRegister>();
	auto fr = std::make_shared<FunctionReg>(tr);
	
	registerStandardTypes(tr.get());
	registerStandardFunctions(fr.get());

	Derivation derivation(tr,fr);


	/* L-sys grammar */

	// int -> int float 
	// float -> int
	
	auto intValue = tr->sharedResource("int");
	auto floatValue  = tr->sharedResource("float");

	std::vector<std::shared_ptr<Resource>> symbols = {intValue};

	derivation.setStartSymbols(symbols);

	// Add rule
	
	derivation.addRule(tr->getTypeId("int"), std::make_shared<AlwaysTrue>(), std::make_shared<ruleInt>(&derivation));
	derivation.addRule(tr->getTypeId("float"), std::make_shared<AlwaysTrue>(), std::make_shared<ruleFloat>(&derivation));

	derivation.generate(atoi(argv[1]));
	
	derivation._debug();


	return 0;
}
