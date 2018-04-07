#ifndef _PARSER_UTILS_H
#define _PARSER_UTILS_H
#include <stack>
#include <vector>
#include <procgen/derivation/derivation.h>

namespace ProcGen {

    using Argument = std::shared_ptr<Function>;
    class ArgumentVector: private std::vector<std::vector<Argument>>
    {
        public:
            void pushArgumentLevel() {
                this->push_back(std::vector<Argument>{});
            }
            std::vector<Argument> popArgumentLevel()
            {
                auto returnValue = this->back();
                this->pop_back();
                return returnValue;
            }

            void pushArgument(Argument arg) {this->back().push_back(arg); }

    };

    class BodyStack: private std::stack<std::shared_ptr<Body>>
    {
        public:
        bool pushBody()
        {
            this->push(std::make_shared<Body>());
	    return true;
        }

        std::shared_ptr<Body> popBody()
        {
            auto top = this->top();
            this->pop();
            return top;
        }
        std::shared_ptr<Body> getTop() { return this->top(); };

    };

	class sTypeDeclaration {
		public:
		std::shared_ptr<Resource>   resource;
		std::string                 name;
	};

    struct Rule {
        std::shared_ptr<Resource>   conditionReturn;
        TypeId                      thisTypeId;
	std::shared_ptr<Resource>   thisValue;
    };


};

#endif
