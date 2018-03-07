#ifndef _NATIVES_H
#define _NATIVES_H
// Native functions for derivation

#include <procgen/interpret/function.h>
#include <procgen/derivation/derivation.h>
#include <procgen/utils/logger.h>

// int = getCurrentPosition() returns current position in derivation string
class NativeCurrentPosition: public Function
{
	std::shared_ptr<Derivation> derivation;
	public:
		NativeCurrentPosition(std::shared_ptr<Derivation> de)
		{
			this->derivation = de;
            this->bindOutput(derivation->tr->sharedResource("int"));
		}
	
		bool operator()(RunStatus& rs)
		{
            *(int*) this->getOutput()->getData() =
                                            derivation->getCurrentStringPositionId();
			return false;
		}
};

// int = getCurrentStringId() returns current string id
class NativeCurrentStringId: public Function
{
	std::shared_ptr<Derivation> derivation;
	public:
		NativeCurrentStringId(std::shared_ptr<Derivation> de)
		{
			this->derivation = de;
            this->bindOutput(derivation->tr->sharedResource("int"));
		}
	
		bool operator()(RunStatus& rs)
		{
            *(int*) this->getOutput()->getData() =
                                     derivation->getCurrentStringId();
			return false;
		}
};


// int = getSymbol(stringID,posID) returns symbol at [stringId,posid]
class NativeGetSymbol: public Function
{
	std::shared_ptr<Derivation> derivation;
	public:
		NativeGetSymbol(std::shared_ptr<Derivation> de)
		{
			this->derivation = de;
		}
	
		bool operator()(RunStatus& rs)
		{
            if(_doInputs(rs)) return true;

            assert(_getInput(0) != nullptr);
            assert(_getInput(0)->getOutput()->getData() != nullptr);
            assert(_getInput(1) != nullptr);
            assert(_getInput(1)->getOutput()->getData() != nullptr);

            int stringId = *(int*) _getInput(0)->getOutput()->getData();
            int positionId = *(int*) _getInput(1)->getOutput()->getData();
    
            this->bindOutput(derivation->getSymbolAtPosition(stringId, positionId));
			return false;
		}
};

class NativeGetParent: public Function
{
	std::shared_ptr<Derivation> derivation;
	public:
		NativeGetParent(std::shared_ptr<Derivation> de)
		{
			this->derivation = de;
            this->bindOutput(derivation->tr->sharedResource("int"));
		}
	
		bool operator()(RunStatus& rs)
		{
            if(_doInputs(rs)) return true;

            assert(_getInput(0) != nullptr);
            assert(_getInput(0)->getOutput()->getData() != nullptr);
            assert(_getInput(1) != nullptr);
            assert(_getInput(1)->getOutput()->getData() != nullptr);

            int stringId = *(int*) _getInput(0)->getOutput()->getData();
            int positionId = *(int*) _getInput(1)->getOutput()->getData();
    
            LOG_DEBUG("getParent() stringId: %d position %d\n", stringId, positionId);

            *(int*) this->getOutput()->getData() =
                        (derivation->getParentId(stringId, positionId));
			return false;
		}
};
#endif
