#include "../interpret/interpret.h"
#include "derivation.h"

class AppendSymbol : public Function
{
	std::shared_ptr<Derivation> derivation;
	public:
		AppendSymbol(std::shared_ptr<Derivation> de)
		{
			this->derivation = de;
		}
	
		bool operator()(RunStatus& rs)
		{

			std::shared_ptr<Resource> src = derivation->tr->sharedResource(_getInput(0)->getOutput()->getBaseId());
			src->copy(_getInput(0)->getOutput());
			std::cout << "Appending id " << src->getBaseId() << "\n";
			derivation->appendNextSymbol(src);

			return false;
		}
};