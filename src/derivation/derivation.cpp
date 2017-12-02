#include "derivation.h"

void Derivation::addRule(TypeId type, std::shared_ptr<Function> predicate, std::shared_ptr<Function> procedure)
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
	bool shouldContinue = false;
	size_t step = 0;
	do {
	std::cout << "Step: " <<step++ <<std::endl;
	shouldContinue = false;
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
				applyRule(availableRules[randomChoose], sym);
			} else {
				std::cerr << "No aplicable rule found for type: " << sym->getName() << std::endl;
				this->appendNextSymbol(sym);
			}
		} else {
			std::cerr << "No rule found for type: " << sym->getName() << std::endl;
			this->appendNextSymbol(sym);
		}
	}
	this->currentString = this->nextString;
	this->nextString.clear();
	} while (shouldContinue && step < maxSteps);
}

bool Derivation::isRuleAplicable(const ruleType & rule, std::shared_ptr<Resource> symbol) const
{
	auto boolResource = this->tr->sharedResource("bool");	
	std::get<0>(rule)->bindInput(0, fr->getHandler(symbol));
	std::get<0>(rule)->bindOutput(boolResource);
	RunStatus rs;
	(*(std::get<0>(rule)))(rs);
	return *(bool*) boolResource->getData();
}

bool Derivation::applyRule(const ruleType & rule, std::shared_ptr<Resource> symbol)
{
	std::get<1>(rule)->bindInput(0, this->fr->getHandler(symbol));
	RunStatus rs;
	(*std::get<1>(rule))(rs);
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
}

void Derivation::_debug()
{
	// Print content of currentString
	std::cout << "Current String Content" << std::endl;
	for(size_t i = 0; i < this->currentString.size(); i++)
	{
		std::cout << i << "\t'" << this->currentString[i]->getName() << "'"<<std::endl;
	}
	std::cout << "Elements count: " << this->currentString.size() << std::endl;
}

