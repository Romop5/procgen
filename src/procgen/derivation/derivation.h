/**
 * @file 'derivation/derivation.h'
 * @brief Carries out derivation (rewriting) process header
 * @copyright The MIT license 
 */
#ifndef DERIVATION_H
#define DERIVATION_H
#include <map>
#include <procgen/interpret/interpret.h>
#include <procgen/utils/json.hpp>

namespace ProcGen {
class Derivation {
public:
    using ruleType = std::tuple<std::shared_ptr<Function>, std::shared_ptr<Function>>;
    std::weak_ptr<FunctionReg> fr;
    std::weak_ptr<TypeRegister> tr;

private:
    size_t currentIterationID;
    size_t currentStringID;
    size_t currentStringPositionID;

    int allowedIterations;

    int numberOfSymbolsToSkip;

    using vectorOfSymbols = std::vector<std::shared_ptr<Resource>>;

    // stores hierarchy of history
    std::map<size_t, vectorOfSymbols> hierarchy;

    // hierarchyRelation defines parent for positions in given string
    // <string,position> -> [string-1] <position>
    std::map<std::pair<size_t, size_t>, size_t> hierarchyRelation;

    std::map<TypeId, std::vector<ruleType>> rules;
    vectorOfSymbols currentString;
    vectorOfSymbols nextString;

    // Object model tree
    //Dom dom;

public:
    Derivation(std::shared_ptr<TypeRegister> tr, std::shared_ptr<FunctionReg> fr)
    {
        this->tr = tr;
        this->fr = fr;
        this->allowedIterations = -1;

        this->currentStringPositionID = 0;
        this->currentStringID = 0;
        this->currentIterationID = 0;
    }

    // Add rule for symbol
    bool addRule(TypeId type, std::shared_ptr<Function> predicate, std::shared_ptr<Function> procedure);

    bool hasAnyRule(TypeId type) const;

    int applyRule(const ruleType& rule, std::shared_ptr<Resource> symbol);

    bool isRuleAplicable(const ruleType& rule, std::shared_ptr<Resource> symbol) const;

    void appendNextSymbol(std::shared_ptr<Resource> symbol);
    // Set symbols
    void setStartSymbols(std::vector<std::shared_ptr<Resource>> symbols);

    // Run generation
    void generate(size_t steps = -1);

    // Move next to current
    void shiftBuffers();
    // Print the status into stdout
    void _debug();

    json to_json() const;

    const std::vector<std::shared_ptr<Resource>> getCurrentSymbolList() const { return this->currentString; }
    void clear()
    {
        this->currentString.clear();
        this->nextString.clear();
    }

    void setMaximumIterations(int maximum) { this->allowedIterations = maximum; }
    void skipSymbol() { this->numberOfSymbolsToSkip++; }

    size_t getCurrentStringId() const { return this->currentStringID; }
    size_t getCurrentStringPositionId() const { return this->currentStringPositionID; }
    size_t getCurrentIterationId() const { return this->currentIterationID; }

    bool hasSymbolAtPosition(size_t stringId, size_t position)
    {
        if (stringId == this->getCurrentIterationId()) {
            if (this->currentString.size() > position)
                return true;
        } else if (this->hierarchy.find(stringId) != this->hierarchy.end()) {
            if (this->hierarchy[stringId].size() > position)
                return true;
        }
        return false;
    }

    std::shared_ptr<Resource> getSymbolAtPosition(size_t stringId, size_t position)
    {
        if (this->hasSymbolAtPosition(stringId, position)) {
            if (stringId == this->getCurrentIterationId()) {
                return this->currentString[position];
            } else if (this->hierarchy.find(stringId) != this->hierarchy.end()) {
                return this->hierarchy[stringId][position];
            }
        }
        return tr.lock()->sharedResource("any");
    }
    size_t getParentId(size_t stringId, size_t position)
    {
        auto key = std::make_pair(stringId, position);
        if (hierarchyRelation.find(key) != hierarchyRelation.end()) {
            return hierarchyRelation[key];
        }
        return -1;
    }
    json getSymbolAtIndex(size_t index)
    {
        if (this->currentString.size() > index) {
            return this->currentString[index]->to_json();
        }
        return json();
    }
    size_t getCountOfSymbols() const { return this->currentString.size(); }
};
}
#endif
