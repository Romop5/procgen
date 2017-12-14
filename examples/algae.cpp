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


void ruleInt(std::shared_ptr<Derivation> der, std::shared_ptr<TypeRegister> tr, std::shared_ptr<FunctionReg> fr)
{

	auto typeInt = tr->sharedResource("int");
	auto typeFloat= tr->sharedResource("float");

	auto appendA = std::make_shared<AppendSymbol>(der);
	appendA->bindInput(0, fr->getHandler(typeInt));

	auto appendB = std::make_shared<AppendSymbol>(der);
	appendB->bindInput(0, fr->getHandler(typeFloat));


	auto body = std::make_shared<Body>();
	body->stats.push_back(appendA);
	body->stats.push_back(appendB);

	fr->addCompositeFunction("ruleInt", body,{typeInt}, nullptr);
}



void ruleFloat(std::shared_ptr<Derivation> der, std::shared_ptr<TypeRegister> tr, std::shared_ptr<FunctionReg> fr)
{

	auto typeFloat = tr->sharedResource("float");
	auto typeInt = tr->sharedResource("int");

	auto append = std::make_shared<AppendSymbol>(der);
	append->bindInput(0, fr->getHandler(typeInt));

	auto body = std::make_shared<Body>();
	body->stats.push_back(append);

	fr->addCompositeFunction("ruleFloat", body,{typeFloat}, nullptr);
}

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
	auto floatValue  = tr->sharedResource("float");

	std::vector<std::shared_ptr<Resource>> symbols = {intValue};

	derivation->setStartSymbols(symbols);

	// Add rule
	
	ruleInt(derivation,tr,fr);
	ruleFloat(derivation,tr,fr);

	auto ruleForInt = fr->getFunc("ruleInt");
	auto ruleForFloat = fr->getFunc("ruleFloat");

	//derivation.addRule(tr->getTypeId("int"), std::make_shared<AlwaysTrue>(), std::make_shared<ruleInt>(&derivation));
	//derivation.addRule(tr->getTypeId("float"), std::make_shared<AlwaysTrue>(), std::make_shared<ruleFloat>(&derivation));

	derivation->addRule(tr->getTypeId("int"), std::make_shared<AlwaysTrue>(),ruleForInt );
	derivation->addRule(tr->getTypeId("float"), std::make_shared<AlwaysTrue>(), ruleForFloat);
	derivation->generate(atoi(argv[1]));
	
	//derivation._debug();
	//
	auto syms= derivation->getCurrentSymbolList();

	auto str = convertSymbolsToString(tr->getTypeId("int"), syms);

	std::cout << str << std::endl;


	return 0;
}
