#ifndef DERIVATION_H
#define DERIVATION_H
#include "interpret.h"
#include <map>

class DomNode {
	public:
		std::shared_ptr<Resource> symbol;
		std::shared_ptr<DomNode> parent;
		std::shared_ptr<DomNode> left;
		std::shared_ptr<DomNode> right;
};

class Dom
{
	public:
		std::map<std::shared_ptr<Resource>, size_t> resourceToId;
		std::map<size_t, DomNode> node;

	bool insertResource(std::shared_ptr<Resource> res)
	{
		static size_t index = 1;	
		resourceToId[res] = index;
		node[index] = DomNode();
		index++;
		return true;

	}
	size_t getResourceId(std::shared_ptr<Resource> res) 
	{
		if(resourceToId.count(res) > 0)
			return resourceToId[res];
		return 0;
	}
	DomNode& getNodeById(size_t id)
	{
		if(node.count(id) > 0)
			return node[id];
		static DomNode empty;
		return empty;
	}
};

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
	
	// Object model tree
	Dom dom;


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
