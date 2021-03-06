/**
 * @file 'procgen.h'
 * @brief API class for ProcGen library header
 * @copyright The MIT license 
 */
/*
 * API
 */
#ifndef PROCGEN_H
#define PROCGEN_H

#include <procgen/parser/generation.h>

/*!
 *  \addtogroup ProcGen 
 *  @{
 */
namespace ProcGen {
class Procgen {
    ProcGen::Generation generation;
    std::string lastError;

public:
    Procgen();
    ~Procgen();

    bool hasAnyErrorMessage() const;

    const std::string& getLastErrorMessage() const;

    bool parseFile(const std::string& file);

    // Run inicialization
    bool runInit();

    // Run real derivation
    bool run(int maximumSteps);

    json serialize() const;

    // Get symbol at index
    json at(size_t index) const;

    bool isReady() { return (generation.hasAnyError == false) && this->isParsed(); }
    bool isParsed() { return generation.flagIsParsed; }

    template <typename T>
    bool setUniform(std::string uniformName, T value)
    {
        return generation.setUniform(uniformName, value);
    }

    template <typename T>
    bool getUniform(std::string uniformName, T* value)
    {
        return generation.getUniform(uniformName, value);
    }

    void setDebugOn(bool state);

    // Clean
    void reinitialize();

    size_t countOfSymbols() const;
    bool appendSymbol(json symbol);
};
}
/*! @} End of Doxygen ProcGen*/
#endif
