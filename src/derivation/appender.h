#include "../interpret/interpret.h"
#include "derivation.h"

class AppendSymbol : public Function
{
	public:
		AppendSymbol(std::shared_ptr<Derivation> de)
		{
			std::shared_ptr<Resource> src = de->tr.sharedResource(_getInput(0)->getBaseId());
			src->copy(_getInput(0));
			de->appendNextSymbol(src);
		}
};
