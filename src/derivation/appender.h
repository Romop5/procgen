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
		    if(_getInput(0) != nullptr)
		    {
			(*_getInput(0))(rs);
		    }

		    std::string dmp = _getInput(0)->getOutput()->to_json().dump();           
		    
		    std::cout << "AppendSymbol: '" << dmp << "'" << std::endl;

			std::shared_ptr<Resource> src = derivation->tr->sharedResource(_getInput(0)->getOutput()->getBaseId());
			if(src->copy(_getInput(0)->getOutput()) == false)
				return false;
			std::cout << "Appending id " << src->getBaseId() << "\n";
			derivation->appendNextSymbol(src);

			return false;
		}
};
