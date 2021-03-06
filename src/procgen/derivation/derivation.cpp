/**
 * @file 'derivation/derivation.cpp'
 * @brief Definition of derivation class
 * @copyright The MIT license 
 */
#include <procgen/derivation/derivation.h>

namespace ProcGen {
bool Derivation::addRule(TypeId type, std::shared_ptr<Function> predicate, std::shared_ptr<Function> procedure)
{
    auto rule = std::make_tuple(predicate, procedure);
    this->rules[type].push_back(rule);
    return true;
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
    // Move next to current buffer etc
    this->shiftBuffers();
    do {
        shouldContinue = false;
        for (size_t i = 0; i < this->currentString.size(); i++) {
            // Reset skip step to 1 - current symbol
            this->numberOfSymbolsToSkip = 0;
            this->currentStringPositionID = i;
            // Find a rule for current symbols
            // If no rule exists, copy to nextString and continue
            // Otherwise, set shouldContinue to TRUE and apply rule

            auto& sym = this->currentString[i];
            if (this->hasAnyRule(sym->getBaseId())) {
                auto& rulesForSymbol = this->rules[sym->getBaseId()];
                // Fill set 'availableRules' with rules whose predicate
                // return 'TRUE'
                std::vector<ruleType> availableRules;
                for (auto& rule : rulesForSymbol) {
                    if (isRuleAplicable(rule, sym)) {
                        availableRules.push_back(rule);
                    }
                }

                // If there is at least one applicable rule
                if (availableRules.size() > 0) {
                    shouldContinue = true;
                    size_t randomChoose = rand() % availableRules.size();
                    // apply rule on symbol
                    applyRule(availableRules[randomChoose], sym);
                    // Skip following symbols if neccessary
                    i += this->numberOfSymbolsToSkip;
                } else {
                    // if 'availableRules' is empty, apply default rule (copy)
                    LOG_DEBUG("No aplicable rule found for type:\n", sym->getTypeName().c_str());
                    this->appendNextSymbol(sym);
                }
            } else {
                // if 'availableRules' is empty, apply default rule (copy)
                LOG_DEBUG("No rule found for type: %s\n", sym->getTypeName().c_str());
                this->appendNextSymbol(sym);
            }
        }
        // Move next to current buffer
        this->shiftBuffers();
    } while (shouldContinue && ++step < maxSteps);
}

bool Derivation::isRuleAplicable(const ruleType& rule, std::shared_ptr<Resource> symbol) const
{
    auto boolResource = this->tr.lock()->sharedResource("bool");
    std::get<0>(rule)->bindInput(0, fr.lock()->getHandler(symbol));
    if (std::get<0>(rule)->getOutput() == nullptr)
        std::get<0>(rule)->bindOutput(boolResource);
    RunStatus rs;
    (*(std::get<0>(rule)))(rs);
    return *(bool*)std::get<0>(rule)->getOutput()->getData();
    //return *(bool*) boolResource->getData();
}

int Derivation::applyRule(const ruleType& rule, std::shared_ptr<Resource> symbol)
{
    std::get<1>(rule)->bindInput(0, this->fr.lock()->getHandler(symbol));

    auto result = std::get<1>(rule)->getOutput();
    *(int*)result->getData() = 0;

    RunStatus rs;
    (*std::get<1>(rule))(rs);

    return 0;
}

bool Derivation::hasAnyRule(TypeId type) const
{
    auto it = this->rules.find(type);
    if (it != this->rules.end())
        return true;
    return false;
}

void Derivation::appendNextSymbol(std::shared_ptr<Resource> symbol)
{
    this->nextString.push_back(symbol);

    // add relation for parent
    hierarchyRelation[std::make_pair(this->getCurrentStringId() + 1, this->nextString.size() - 1)] = this->getCurrentStringPositionId();
}

void Derivation::shiftBuffers()
{
    // Increment currentString to match next string
    this->currentStringID++;
    // Add next string to hierarchy
    this->hierarchy[this->currentStringID] = this->nextString;

    // Swap buffers
    this->currentString = this->nextString;
    this->nextString.clear();

    // Reset position
    this->currentStringPositionID = 0;
}

void Derivation::_debug()
{
    // Print content of currentString
    std::cout << "Current String Content" << std::endl;
    for (size_t i = 0; i < this->currentString.size(); i++) {
        std::cout << i << "\t'" << this->currentString[i]->getTypeName() << "'" << std::endl;
    }
    std::cout << "Elements count: " << this->currentString.size() << std::endl;
}

json Derivation::to_json() const
{
    json subjson;

    for (auto& x : this->currentString) {
        subjson.push_back(x->to_json());
    }
    return subjson;
}
}
