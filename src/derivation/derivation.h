#ifndef DERIVATION_H
#define DERIVATION_H
#include "interpret.h"

class Derivation
{
	public:
	using ruleType = std::tuple<std::shared_ptr<Function>, std::shared_ptr<Function> >;
	std::shared_ptr<FunctionReg> fr;
	std::shared_ptr<TypeRegister> tr;

	private:
	std::map<TypeId, std::vector<ruleType>> rules;
	std::vector<std::shared_ptr<Resource>> currentString;
	std::vector<std::shared_ptr<Resource>> nextString;
	public:
	Derivation(std::shared_ptr<TypeRegister> tr, std::shared_ptr<FunctionReg> fr)
	{
		this->tr = tr;
		this->fr = fr;
	}

	// Add rule for symbol
	void addRule(TypeId type, std::shared_ptr<Function> predicate, std::shared_ptr<Function> procedure);

	bool hasAnyRule(TypeId type) const;

	bool applyRule(const ruleType & rule, std::shared_ptr<Resource> symbol);

	bool isRuleAplicable(const ruleType & rule, std::shared_ptr<Resource> symbol) const;

	void appendNextSymbol(std::shared_ptr<Resource> symbol);
	// Set symbols
	void setStartSymbols(std::vector<std::shared_ptr<Resource>> symbols);

	// Run generation
	void generate(size_t steps = -1);

	// Print the status into stdout
	void _debug();

	const std::vector<std::shared_ptr<Resource>> getCurrentSymbolList() const
	{
		return this->currentString;
	}

};
#endif
