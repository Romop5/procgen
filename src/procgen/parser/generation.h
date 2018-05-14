/**
 * @file 'parser/generation.h'
 * @brief Generation class which maps Bison grammar to interpret and derivator
 * @copyright The MIT license 
 */
/*
 * Compilation helper structs / classes
 */
#ifndef GENERATION_H
#define GENERATION_H

#include <procgen/derivation/derivation.h>
#include <procgen/parser/scanner.h>
#include <procgen/parser/utils.h>

#include "location.hh"

#include <procgen/utils/json.hpp>
#include <stack>
#include <string>

namespace ProcGen {

// fwd dec
class Parser;
class Scanner;
class Location;

class Generation {
public:
    Scanner* _scanner;
    Parser* _parser;
    location _location;

    friend class Parser;
    friend class Scanner;

    std::shared_ptr<TypeRegister> typeregister;
    std::shared_ptr<FunctionReg> functionregister;
    // Stores all local variables (local to function, rule)
    std::shared_ptr<VariableReg> localStackFrame;
    std::shared_ptr<VariableReg> globalVariables;
    std::shared_ptr<Derivation> der;
    bool flagIsParsed;
    bool hasAnyError;
    Generation();
    ~Generation();

    /**
     * @brief Sets the value of variable defined as parameter in script
     *
     * @tparam T
     * @param uniformName Name of parameter as defined in script
     * @param value New value
     * @return true if value was set correctly
     *
     * This function sets the value of uniformName to value. 
     * It only works for atomic resources (bool, int, float).
     */
    template <typename T>
    bool setUniform(std::string uniformName, T value)
    {
        auto var = globalVariables->getVar(uniformName);
        if (!var)
            return false;

        if (var->getResourceType() != ResourceType::ATOMIC)
            return false;

        if (keyword<T>() != var->getTypeName())
            return false;
        *(T*)var->getData() = value;
        return true;
    }

    /**
     * @brief Gets the value of variable defined as parameter in script
     *
     * @tparam T
     * @param uniformName Name of parameter as defined in script
     * @param value Return value
     * @return true if value was get correctly
     *
     * This function gets the value of uniformName. 
     * It only works for atomic resources (bool, int, float).
     */
    template <typename T>
    bool getUniform(std::string uniformName, T* value)
    {
        auto var = globalVariables->getVar(uniformName);
        if (!var)
            return false;

        if (var->getResourceType() != ResourceType::ATOMIC)
            return false;

        if (keyword<T>() != var->getTypeName())
            return false;
        *value = *(T*)var->getData();
        return true;
    }

    bool appendSymbol(json symbol);

    /* Compilation utilities*/

    /**
     * @brief List of structure members (pair name / type) 
     */
    std::vector<sTypeDeclaration> typeList;

    /**
     * @brief Stack of Function used for expression parsing
     */
    std::stack<std::shared_ptr<Function>> expressionsStack;

    /**
     * @brief Stack of Body used for handling the structure of structured code
     */
    BodyStack stackedBodies;

    /**
     * @brief List of function arguments (for function call)
     */
    ArgumentVector argumentVector;

    Rule ruleDefinition;

    /**
     * @brief Parse a file 'file'
     *
     * @param file
     *
     * @return true on success, false on error
     */
    bool parseFile(const std::string& file);

    /**
     * @brief Run script init() function
     *
     * @return false when init() is missing
     */
    bool runInit();

    /**
     * @brief Run derivation process
     *
     * @param maximumSteps
     *
     * @return 
     */
    bool run(int maximumSteps);

    /**
     * @brief Return JSON object containing the list of symbols, which came from generation
     *
     * @return JSON object
     */
    json serialize() const;

    /**
     * @brief Is parsed without error ?
     *
     * @return 
     */
    bool isReady() { return flagIsParsed; }

    /**
     * @brief Determines if any error occured during compilation
     *
     * @return 
     */
    bool hasAnyCompilationError() { return this->hasAnyError; }

    /**
     * @brief Error reporting function similar to printf
     *
     * @param message
     * @param ...
     *
     * @return 
     */
    bool errorMessage(std::string message, ...);

    void setDebugOn(bool state);

    /*
		 *	Utility functions, called by parser
		 */

    /**
     * @brief Called during initialization, register all standard types / functions
     */
    void registerNatives();
    // Prepare local stack
    bool initializeFunction(const char* type);

    bool registerRule(const char* name, const char* type);

    // Register a new name for type.
    // May fail on missing aliasedType or alias name collision
    bool registerAlias(const char* alias, const char* aliasedType);

    // Define a structure with its members. May fail on invalid type / name duplication
    bool registerStruct(const char* name, std::vector<sTypeDeclaration>& types);

    // Register global parameter
    bool registerParameter(const char* name, const char* type, bool hasLiteral);

    bool registerFunction(const char* type, const char* name);

    std::shared_ptr<Function> createUnaryOperation(const char operation);
    std::shared_ptr<Function> createExpressionOperation(const char operation);
    std::shared_ptr<Function> createExpressionLogicOperation(const char operation);

    void createLiteralBool(bool value);
    void createLiteralInteger(int value);
    void createLiteralFloat(float value);
    bool createLiteralFromVariable(const char* name);
    bool createStructuredLiteral(const char* member);

    bool createFunctionCall(const char* functionName, std::vector<Argument>);

    // Replace expression with argument
    bool createArgument();
    bool registerLocalVariable(const char* type, const char* name, bool hasExp);
    bool makeReturn(bool hasExpression);

    bool makeAssignment(const char* name, bool hasAssignment, const char op = '=');

    bool makeWhile();

    bool makeIfStatement(bool hasElseBranch);

    bool makeCallStatement();

    bool hasType(const char* name);

    bool registerFormalParameter(sTypeDeclaration& parameter);

    sTypeDeclaration fillTypeDeclaration(const char* type, const char* name);

    bool initializeRule(const char* typeName);
    bool ruleProcedure(const char* rulename);

    bool makeTypeid(const char* name);
    bool makeConvert(const char* name);

    bool makeExplicitCast(const char* finalTypename);

    bool makeConstructor(const char* functionName, std::vector<Argument>);

    bool createCollectionInsert();
    bool createCollectionAt();
    bool createCollectionSize();
    bool createCollectionDel();

    bool isLogicOperator(const char operatorName)
    {
        switch (operatorName) {
        case '>':
        case '<':
        case '=':
        case '!':
            return true;
        default:
            return false;
        }
        return false;
    }

private:
    std::shared_ptr<Resource> getVariable(std::string name);
};
}
#endif
