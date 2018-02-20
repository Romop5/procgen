#include <procgen/derivation/derivation.h>

class AlwaysTrue : public Function
{
	public:
	virtual bool operator()(RunStatus& rs)
	{
		*(bool*) getOutput()->getData() = true;
		return false;
	}
};

class TestInt : public Function
{
	Derivation* der;
	public:
	TestInt(Derivation* der)
	{
		this->der = der;
	}
	virtual bool operator()(RunStatus& rs)
	{
		std::cout << "Symbol: " << *(int*) _getInput(0)->getOutput()->getData() << std::endl;
		return false;
	}
};

class CopyInt : public Function
{
	Derivation* der;
	public:
	CopyInt(Derivation* der)
	{
		this->der = der;
	}
	virtual bool operator()(RunStatus& rs)
	{
		this->der->appendNextSymbol(_getInput(0)->getOutput());
		return false;
	}
};

int main()
{
	srandom(time(NULL));
	
	auto tr = std::make_shared<TypeRegister>();
	auto fr = std::make_shared<FunctionReg>(tr);
	
	registerStandardTypes(tr.get());
	registerStandardFunctions(fr.get());

	Derivation derivation(tr,fr);

	/* LL grammer */

	// int -> 
	
	auto intValue = tr->sharedResource("int");
	auto floatValue  = tr->sharedResource("float");

	std::vector<std::shared_ptr<Resource>> symbols = {intValue, floatValue};

	derivation.setStartSymbols(symbols);

	// Add rule
	
	derivation.addRule(tr->getTypeId("int"), std::make_shared<AlwaysTrue>(), std::make_shared<TestInt>(&derivation));
	derivation.addRule(tr->getTypeId("int"), std::make_shared<AlwaysTrue>(), std::make_shared<CopyInt>(&derivation));

	derivation.generate(1);
	
	derivation._debug();


	return 0;
}
