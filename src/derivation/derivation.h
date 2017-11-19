#include "interpret.h"

class Derivation
{
	std::shared_ptr<FunctionReg> fr;
	std::shared_ptr<TypeReg> tr;
	public:
	Derivation(std::shared_ptr<TypeReg> tr, std::shared_ptr<FunctionReg> fr)
	{
		this->tr = tr;
		this->fr = fr;
	}

	// Add rule for symbol
	void addRule(TypeId type, std::shared_ptr<Function> predicate, std::shared_ptr<Function> procedure);

	// Set symbols
	void setStartSymbols(std::vector<std::shared_ptr<Resource>> symbols);


	// Run generation
	void generate();

	// Print the status into stdout
	void _debug();

};
