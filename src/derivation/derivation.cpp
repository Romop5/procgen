#include "derivation.h"

bool Derivation::addRule(TypeId type, std::shared_ptr<Function> predicate, std::shared_ptr<Function> procedure)
{
	auto rule = std::make_tuple(predicate,procedure);
	this->rules[type].push_back(rule);
}

void Derivation::setStartSymbols(std::vector<std::shared_ptr<Resource>> symbols)
{
	this->currentString = symbols;
}
	
void Derivation::generate(size_t maxSteps)
{
    maxSteps = this->allowedIterations;
	bool shouldContinue = false;
	size_t step = 0;
    // Move next to current buffer
    this->shiftBuffers();
	do {
	std::cout << "Step: " <<step++ <<std::endl;
	shouldContinue = false;
	std::cout << "Current string size: " << this->currentString.size() <<std::endl;
	for(size_t i = 0; i < this->currentString.size(); i++)
	{
		// Find a rule for current symbols
		// If no rule exists, copy to nextString and continue
		// Otherwise, set shouldContinue to TRUE and apply rule	

		auto& sym = this->currentString[i];
		if(this->hasAnyRule(sym->getBaseId()))
		{
			auto& rulesForSymbol = this->rules[sym->getBaseId()];
			std::vector<ruleType> availableRules;
			for(auto& rule: rulesForSymbol)
			{
				if(isRuleAplicable(rule, sym))
				{
					availableRules.push_back(rule);
				}
			}
			// choose based on random
			if(availableRules.size() > 0)
			{
				shouldContinue = true;	
				size_t randomChoose = random() % availableRules.size();
				std::cout << "Random no: "<< randomChoose << std::endl;
				// apply rule on symbol
				int howManytoSkip = applyRule(availableRules[randomChoose], sym);
				// skip symbols
				i += howManytoSkip;	
			} else {
				std::cerr << "No aplicable rule found for type: " << sym->getTypeName() << std::endl;
				this->appendNextSymbol(sym);
			}
		} else {
			std::cerr << "No rule found for type: " << sym->getTypeName() << std::endl;
			this->appendNextSymbol(sym);
		}
	}
        // Move next to current buffer
        this->shiftBuffers();
	} while (shouldContinue && step < maxSteps);
}

bool Derivation::isRuleAplicable(const ruleType & rule, std::shared_ptr<Resource> symbol) const
{
	auto boolResource = this->tr->sharedResource("bool");	
	std::get<0>(rule)->bindInput(0, fr->getHandler(symbol));
	if(std::get<0>(rule)->getOutput() == nullptr)
		std::get<0>(rule)->bindOutput(boolResource);
	RunStatus rs;
	(*(std::get<0>(rule)))(rs);
	return *(bool*) std::get<0>(rule)->getOutput()->getData();
	//return *(bool*) boolResource->getData();
}

int Derivation::applyRule(const ruleType & rule, std::shared_ptr<Resource> symbol)
{
	std::get<1>(rule)->bindInput(0, this->fr->getHandler(symbol));

	auto result = std::get<1>(rule)->getOutput();
	*(int*) result->getData() = 0;

	RunStatus rs;
	(*std::get<1>(rule))(rs);


	std::cout << "Return skip number: " << *(int*) result->getData() << std::endl;
}

bool Derivation::hasAnyRule(TypeId type) const
{
	auto it = this->rules.find(type);
	if(it != this->rules.end())
		return true;
	return false;
}

void Derivation::appendNextSymbol(std::shared_ptr<Resource> symbol)
{
	this->nextString.push_back(symbol);
	std::cout << "Appended\n";
	std::cout << this->nextString.size() << "\n";
	std::cout << this << std::endl;
}

void Derivation::shiftBuffers()
{
    std::cout << this->nextString.size() << std::endl;
	this->currentString = this->nextString;
	this->nextString.clear();

}

void Derivation::_debug()
{
	// Print content of currentString
	std::cout << "Current String Content" << std::endl;
	for(size_t i = 0; i < this->currentString.size(); i++)
	{
		std::cout << i << "\t'" << this->currentString[i]->getTypeName() << "'"<<std::endl;
	}
	std::cout << "Elements count: " << this->currentString.size() << std::endl;
}

json Derivation::to_json() const
{
    json subjson;
    
    for(auto &x : this->currentString)
    {
        subjson.push_back(x->to_json());
    }
    return subjson;
}

