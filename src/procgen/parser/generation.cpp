/**
 * @file 'parser/generation.cpp'
 * @brief Definition of Generation class
 * @copyright The MIT license 
 */
#include "parser.hh"
#include <fstream>
#include <procgen/derivation/natives.h>
#include <procgen/parser/generation.h>
#include <sstream>

#include <cstdarg>

namespace ProcGen {
Generation::Generation()
{
    _scanner = new Scanner();
    _parser = new Parser(*this);

    typeregister = std::make_shared<TypeRegister>();
    functionregister = std::make_shared<FunctionReg>(typeregister);
    localStackFrame = std::make_shared<VariableReg>(typeregister);
    globalVariables = std::make_shared<VariableReg>(typeregister);
    der = std::make_shared<Derivation>(typeregister, functionregister);

    this->registerNatives();

    flagIsParsed = true;
    hasAnyError = false;
}

Generation::~Generation()
{
    delete _parser;
    delete _scanner;
}

bool Generation::appendSymbol(json symbol)
{
    auto resource = this->typeregister->createResourceFromJson(symbol);
    if (resource == nullptr)
        return false;
    this->der->appendNextSymbol(resource);
    return true;
}

bool Generation::parseFile(const std::string& file)
{
    std::ifstream s(file.c_str(), std::ifstream::in);
    _scanner->switch_streams(&s, &std::cerr);
    auto newString = std::make_shared<std::string>(file);
    _location.initialize(newString.get());
    _parser->parse();

    s.close();

    return (!hasAnyError);
}

bool Generation::runInit()
{
    // clear all
    //this->der->clear();
    auto initFunction = functionregister->getFunc("init");
    if (initFunction == nullptr) {
        errorMessage("Missing init() function");
        return false;
    }
    RunStatus rs;
    (*initFunction)(rs);
    return true;
}

bool Generation::run(int maximumSteps)
{
    der->generate(maximumSteps);
    return true;
}

#define REGISTER_NATIVE_FUNCTION(stringName, className)                                         \
    functionregister->addFunction(stringName,                                                   \
        [derivation]() -> std::shared_ptr<Function> {                                           \
            return std::static_pointer_cast<Function>(std::make_shared<className>(derivation)); \
        });

void Generation::registerNatives()
{
    // Register standard types provided by interpret (bool, int, float)
    registerStandardTypes(typeregister.get());
    // Register standard functions by interpret (sin, cos)
    registerStandardFunctions(functionregister.get());

    // Add functions provided by derivation module
    auto derivation = this->der;
    REGISTER_NATIVE_FUNCTION("appendSymbol", NativeAppendSymbol);
    REGISTER_NATIVE_FUNCTION("getCurrentPosition", NativeCurrentPosition);
    REGISTER_NATIVE_FUNCTION("getCurrentStringId", NativeCurrentStringId);
    REGISTER_NATIVE_FUNCTION("getSymbol", NativeGetSymbol);
    REGISTER_NATIVE_FUNCTION("hasSymbol", NativeHasSymbol);
    REGISTER_NATIVE_FUNCTION("getParent", NativeGetParent);
    REGISTER_NATIVE_FUNCTION("setMaximumIterations", NativeIteratorLimit);
    REGISTER_NATIVE_FUNCTION("skipSymbol", NativeSkipSymbol);
}

bool Generation::initializeFunction(const char* type)
{
    this->localStackFrame->clear();
    auto returnResource = typeregister->sharedResource(type);
    if (returnResource == nullptr) {
        errorMessage("Failed to create %s resource. \n", type);
        return false;
    }
    // Add special local variable for returning function value
    this->localStackFrame->addVar("_return", returnResource);
    return true;
}

bool Generation::registerRule(const char* name, const char* type)
{
    static int ruleID = 0;
    ruleID++;

    // Get procedure
    auto procedure = this->stackedBodies.popBody();

    std::stringstream ruleName;
    ruleName << "rule" << ruleID;
    //auto typeResource = typeregister->sharedResource(type);
    auto typeResource = localStackFrame->getVar("this");

    functionregister->addCompositeFunction(
        ruleName.str(), procedure, { typeResource }, localStackFrame->getVar("_return"));

    // Get condition
    auto boolResult = ruleDefinition.conditionReturn;
    auto condition = this->stackedBodies.popBody();

    auto thisResource = this->ruleDefinition.thisValue;
    std::stringstream conditionName;
    conditionName << "condition" << ruleID;
    functionregister->addCompositeFunction(
        conditionName.str(), condition, { thisResource }, boolResult);

    TypeId typeId = typeregister->getTypeId(type);
    return der->addRule(typeId, functionregister->getFunc(conditionName.str()),
        functionregister->getFunc(ruleName.str()));
}
bool Generation::registerAlias(const char* alias, const char* aliasedType)
{
    if (typeregister->addAlias(alias, aliasedType) == false) {
        errorMessage("Failed to register alias %s for type %s\n", alias, aliasedType);
        return false;
    }
    return true;
}

bool Generation::registerStruct(const char* name, std::vector<sTypeDeclaration>& typelist)
{
    LOG_DEBUG("Registering struct %s\n", name);
    std::vector<TypeId> types;
    std::vector<std::string> names;
    for (auto& x : typelist) {
        types.push_back(x.resource->getBaseId());
        names.push_back(x.name);
    }
    // Clear types
    typelist.clear();

    auto result = typeregister->addCompositeWithNames(name, types, names);
    if (result == false)
        errorMessage("Failed to register structure named %s\n", name);
    return result;
}

bool Generation::registerParameter(const char* name, const char* type, bool hasLiteral)
{
    // Get resource for type
    auto resource = typeregister->sharedResource(type);
    if (resource == nullptr) {
        errorMessage("Undefined parameter type %s:%s\n", type, name);
        resource = typeregister->sharedResource("any");
    }

    globalVariables->addVar(name, resource);

    // TODO: check types and do implicit converion if neccesary
    // TODO: use hasLiteral and literal

    // if literal is specified
    if (hasLiteral) {
        auto literal = this->expressionsStack.top();
        // calculate literal and set the param
        auto setter = std::make_shared<GenericCopy>();
        setter->bindInput(0, functionregister->getHandler(resource));
        setter->bindInput(1, literal);

        RunStatus rs;
        (*setter)(rs);

        this->expressionsStack.pop();
    }

    return true;
}

bool Generation::registerFunction(const char* type, const char* name)
{
    LOG_DEBUG("Registering function %s:%s\n", type, name);
    auto resource = localStackFrame->getVar("_return");
    if (resource == nullptr) {
        errorMessage("Unknown type: %s\n", type);
    }

    // Get the top of statement stack
    auto statementTop = this->stackedBodies.popBody();

    // Create a vector of input parameters
    std::vector<std::shared_ptr<Resource>> inputResources;

    // type list
    //std::cout << "Registering function " << name << "with num. of params:"
    //<< typeList.size() << "\n";
    for (auto& typeDesc : typeList) {
        inputResources.push_back(typeDesc.resource);
        //std::cout << "Parameter: " << typeDesc.name << " with type: "
        //<< typeDesc.resource->getTypeName()
        //<< std::endl;
    }

    typeList.clear();

    return functionregister->addCompositeFunction(name,
        statementTop, inputResources, resource);
}

std::shared_ptr<Function> Generation::createUnaryOperation(const char operation)
{
    std::shared_ptr<Function> exp = this->expressionsStack.top();
    this->expressionsStack.pop();

    std::string operationName;
    switch (operation) {
    case '-':
        operationName = "UnaryMinus";
        break;
    case '!':
        operationName = "Negation";
        break;
    }
    auto type = exp->getOutput()->getTypeName();
    auto tmpResult = typeregister->sharedResource(type);
    auto operationBox = functionregister->getFunc(operationName + ":" + type);
    if (operationBox == nullptr) {
        errorMessage("Failed to create semantic action for unary %c operator", operation);
    }
    if (this->isLogicOperator(operation))
        tmpResult = typeregister->sharedResource("bool");

    operationBox->bindInput(0, exp);
    operationBox->bindOutput(tmpResult);

    this->expressionsStack.push(operationBox);
    return operationBox;
}

std::shared_ptr<Function> Generation::createExpressionOperation(const char operation)
{
    // TODO: produce common type

    // 1. get two operands from expression stack
    std::shared_ptr<Function> first = this->expressionsStack.top();
    this->expressionsStack.pop();
    std::shared_ptr<Function> second = this->expressionsStack.top();
    this->expressionsStack.pop();

    // 2. detect the type of operands
    TypeId a = 0;
    if (first->getOutput() != nullptr)
        a = first->getOutput()->getBaseId();

    TypeId b = 0;
    if (second->getOutput() != nullptr)
        b = second->getOutput()->getBaseId();

    // TODO: determien if types are compatible
    // and which type we should use
    // and arrange implicit conversion if neccessary

    if (a != b) {
        std::string firstType = typeregister->getTypeName(a);
        std::string secondType = typeregister->getTypeName(b);

        int common = getCommonType(firstType, secondType);
        if (common == 0) {
            // error
            errorMessage("Semantic error: types don't match. Types:\n\
                    %s, %s\n",
                typeregister->getTypeName(a).c_str(),
                typeregister->getTypeName(b).c_str());
        } else {
            if (common == 2) {
                firstType.swap(secondType);
                std::swap(first, second);
            }
            auto conversionFunction = functionregister->getFunc(std::string("Cast:") + firstType + ":" + secondType);
            if (conversionFunction == nullptr) {
                errorMessage("Semantic error: failed to cast operands to %s from %s", firstType.c_str(), secondType.c_str());
            }
            conversionFunction->bindInput(0, second);
            conversionFunction->bindOutput(typeregister->sharedResource(secondType));
            second = conversionFunction;
        }
    }

    // TODO: watch out for aliases

    std::string type = typeregister->getTypeName(a);
    std::string operationName = "";
    switch (operation) {
    case '+':
        operationName = "Add";
        break;
    case '-':
        operationName = "Sub";
        break;
    case '/':
        operationName = "Div";
        break;
    case '*':
        operationName = "Mul";
        break;
    case '>':
        operationName = "Greater";
        break;
    case '<':
        operationName = "Less";
        break;
    case '=':
        operationName = "Eq";
        break;
    case '!':
        operationName = "NotEq";
        break;
    case '&':
        operationName = "And";
        break;
    case '|':
        operationName = "Or";
        break;
    case '%':
        operationName = "Modulo";
        break;
    default:
        errorMessage("Undefined operation");
    }

    // Create operation function

    //std::cout << type+":"+operationName<< " je kunda" << std::endl;
    auto operationBox = functionregister->getFunc(operationName + ":" + type);
    if (!operationBox) {
        errorMessage("Failed to create an instance for operation %s for type %s", operationName.c_str(), type.c_str());
        operationBox = std::make_shared<Function>();
    }

    auto tmpResult = typeregister->sharedResource(a);
    if (this->isLogicOperator(operation))
        tmpResult = typeregister->sharedResource("bool");

    operationBox->bindInput(0, second);
    operationBox->bindInput(1, first);
    operationBox->bindOutput(tmpResult);

    this->expressionsStack.push(operationBox);
    return operationBox;
}

void Generation::createLiteralBool(bool value)
{
    auto val = typeregister->sharedResource("bool");
    if (!val)
        errorMessage("Unexpected error while creating bool resource");

    *(bool*)val->getData() = value;
    this->expressionsStack.push(functionregister->getHandler(val));
}

void Generation::createLiteralInteger(int value)
{
    auto val = typeregister->sharedResource("int");
    if (!val)
        errorMessage("Unexpected error while creating int resource");

    *(int*)val->getData() = value;
    this->expressionsStack.push(functionregister->getHandler(val));
}

void Generation::createLiteralFloat(float value)
{
    auto val = typeregister->sharedResource("float");
    if (!val)
        errorMessage("Unexpected error while creating flaot resource");

    *(float*)val->getData() = value;
    this->expressionsStack.push(functionregister->getHandler(val));
}

bool Generation::createLiteralFromVariable(const char* name)
{
    auto res = this->getVariable(name);
    if (res == nullptr) {
        errorMessage("Undefined variable %s", name);
        res = typeregister->sharedResource("int");
    }
    this->expressionsStack.push(functionregister->getHandler(res));
    return true;
}

bool Generation::createStructuredLiteral(const char* member)
{
    auto compositeFunction = this->expressionsStack.top();
    this->expressionsStack.pop();

    auto resource = compositeFunction->getOutput();
    if (resource == nullptr) {
        errorMessage("Failed to get resource");
    }
    if (resource->getResourceType() != ResourceType::COMPOSITE) {
        errorMessage("Isn't composite at all.");
    }
    auto compositeResource = std::dynamic_pointer_cast<CompositeResource>(resource);

    auto structureName = compositeResource->getTypeName();
    auto position = compositeResource->getComponentPosition(member);

    if (position == (size_t)-1) {
        errorMessage("'%s' doesn't contain name '%s'", structureName.c_str(), member);
    }

    TypeId memberType = compositeResource->getComponentType(position);

    // set index
    auto indexResoure = typeregister->sharedResource("int");
    *(int*)indexResoure->getData() = position;
    // create accessor
    auto getter = std::make_shared<CompositeGet>();
    getter->bindInput(0, compositeFunction);
    getter->bindInput(1, functionregister->getHandler(indexResoure));

    // This is actually useless for interpreter as it will replace
    // output resource when operator() is run
    // However, it's neccessary for parser to get base structure metadata
    auto outputResource = typeregister->sharedResource(memberType);
    getter->bindOutput(outputResource);

    //std::cout << "Pushing structuremember '" << member << "' with type " <<
    //outputResource->getTypeName() << std::endl;
    this->expressionsStack.push(getter);
    return true;
}

bool Generation::createFunctionCall(const char* functionName, std::vector<Argument> args)
{
    auto functionPointer = functionregister->getFunc(functionName);
    if (functionPointer == nullptr) {
        // if functionName doesn't exist
        errorMessage("Failed to get function '%s'", functionName);
        this->expressionsStack.push(std::make_shared<Function>());
        return false;
    }

    // For all given parameters
    for (unsigned param = 0; param < args.size(); param++) {
        // TODO: semantic control / implicit conversion
        bool isTypeCompatible = functionPointer->bindInput(param, args[param]);
        if (!isTypeCompatible) {
            errorMessage("[Semantic Error] Incompatible parameter %d's type in function call of %s", param, functionName);
        }
    }

    // detect if all required ports are binded

    if (functionPointer->hasRequiredBindings() == false) {
        errorMessage("[Semantic Error] Invalid count of parameters given to call of %s. Expected %d parameters",
            functionName, functionPointer->getNumberOfExpectedInputs());
    }

    // Push new expression(function call) to expression stack
    this->expressionsStack.push(functionPointer);
    return true;
}

bool Generation::createArgument()
{
    std::shared_ptr<Function> expr = this->expressionsStack.top();
    this->expressionsStack.pop();
    assert(expr != nullptr);

    this->argumentVector.pushArgument(expr);
    return true;
}

bool Generation::registerLocalVariable(const char* type, const char* name, bool hasExp)
{
    auto resource = typeregister->sharedResource(type);
    if (resource == nullptr) {
        errorMessage("Invalid type %s\n", type);
        return false;
    }
    this->localStackFrame->addVar(name, resource);

    // if has = expression
    if (hasExp) {
        // create expression with name
        auto swap = this->expressionsStack.top();
        this->expressionsStack.pop();
        // A = B, B must be at top, that's why we need to push it later
        this->expressionsStack.push(functionregister->getHandler(resource));
        this->expressionsStack.push(swap);
        return this->makeAssignment(name, true);
    } else {
        // clear collection
        if (resource->getResourceType() == ResourceType::COLLECTION) {
            auto collectionReseter = std::make_shared<CollectionClear>();
            collectionReseter->bindInput(0, functionregister->getHandler(resource));
            this->stackedBodies.getTop()->appendStatement(collectionReseter);
        }
    }
    return true;
}

bool Generation::makeReturn(bool hasExpression)
{
    auto box = std::make_shared<Return>();
    if (hasExpression) {

        // get resourse
        auto resource = localStackFrame->getVar("_return");
        /*      auto typeName = typeregister->getTypeName(resource->getBaseId());
            auto function = functionregister->getFunc(std::string("Copy")+":"+typeName);

	    

            // Bind return box with resources
            function->bindInput(0,expr);
            function->bindOutput(resource);
*/
        if (resource == nullptr) {
            errorMessage("Failed to get return resource");
        }

        auto expr = this->expressionsStack.top();
        this->expressionsStack.pop();

        auto genericCopy = std::make_shared<GenericCopy>();
        genericCopy->bindInput(0, functionregister->getHandler(resource));
        genericCopy->bindInput(1, expr);

        box->bindInput(genericCopy);
    }
    // Register return
    this->stackedBodies.getTop()->appendStatement(box);

    return true;
}
bool Generation::makeAssignment(const char* name, bool hasAssignment, const char op)
{
    /*// get resourse
		auto resource = localStackFrame->getVar(name);
		auto typeName = typeregister->getTypeName(resource->getBaseId());
		auto function = functionregister->getFunc(std::string("Copy")+":"+typeName);
		if(function == nullptr)
		{
			errorMessage("Failed to create assignment for type %s, variable %s\n",
					typeName.c_str(), name);
		}

        */

    // Get expression
    auto expressionTop = this->expressionsStack.top();
    this->expressionsStack.pop();

    if (!hasAssignment) {
        this->stackedBodies.getTop()->appendStatement(expressionTop);
        return true;
    }

    // Get expression
    auto assignedResource = this->expressionsStack.top();
    this->expressionsStack.pop();

    if (expressionTop == nullptr || assignedResource == nullptr) {
        errorMessage("[Internal error] Nullptr");
        return false;
    }

    // if we don't assign to "any" variable
    if (assignedResource->getOutput()->getTypeName() != "any") {
        // if expession doesn't have the same type
        if (!assignedResource->getOutput()->hasSameType(expressionTop->getOutput())) {
            // try to convert
            auto first = assignedResource->getOutput()->getTypeName();
            auto second = expressionTop->getOutput()->getTypeName();

            LOG_DEBUG("Comparing 1. %s with 2. %s\n", first.c_str(), second.c_str());
            int compatibilityResult = getCommonType(second, first);
            LOG_DEBUG("Compability result %d\n", compatibilityResult);
            // if conversion is impossible
            if (compatibilityResult != 2) {
                errorMessage("[Semantic error] Incompatible types in assignment.");
            }
            // else convert second to first

            auto conversionFunction = functionregister->getFunc("Cast:" + first + ":" + second);
            if (conversionFunction == nullptr) {
                errorMessage("[Semantic error] Incompatible types in assignment - internal error.");
            } else {
                conversionFunction->bindInput(0, expressionTop);
                conversionFunction->bindOutput(typeregister->sharedResource(first));
                expressionTop = conversionFunction;
            }
        }
    }

    std::shared_ptr<Function> resultFunction = nullptr;
    // if it is just a regular assignment
    if (op == '=') {
        resultFunction = std::make_shared<GenericCopy>();
        resultFunction->bindInput(0, assignedResource);
        resultFunction->bindInput(1, expressionTop);
    } else {
        std::string operationName;
        switch (op) {
        case '+':
            operationName = "Add";
            break;
        case '-':
            operationName = "Sub";
            break;
        case '*':
            operationName = "Mul";
            break;
        case '/':
            operationName = "Div";
            break;
        default:
            errorMessage("[Internal] Operator selection");
        }

        resultFunction = functionregister->getFunc(operationName + ":" + assignedResource->getOutput()->getTypeName());
        if (resultFunction == nullptr) {
            errorMessage("[Internal] Failed to get operation function in assignment");
        }
        // dest OP= src;
        resultFunction->bindInput(0, assignedResource); // dest
        resultFunction->bindInput(1, expressionTop); // src
        resultFunction->bindOutput(assignedResource->getOutput()); // dest
    }
    // Register
    assert(resultFunction != nullptr);
    if (this->stackedBodies.count() == 0) {
        errorMessage("[Internal] Failed to get Body() object");
    } else {
        this->stackedBodies.getTop()->appendStatement(resultFunction);
    }

    return true;
}

bool Generation::makeWhile()
{
    // Remove the top of expression stack
    auto expressionTop = this->expressionsStack.top();
    this->expressionsStack.pop();

    // Get the top of statement stack
    auto statementTop = this->stackedBodies.popBody();

    auto whileStatement = std::make_shared<While>();
    whileStatement->bindCondition(expressionTop);
    whileStatement->bindStatement(statementTop);

    // Append while to body
    this->stackedBodies.getTop()->appendStatement(whileStatement);
    return true;
}

bool Generation::makeCallStatement()
{
    // Get expression
    auto expressionTop = this->expressionsStack.top();
    this->expressionsStack.pop();

    this->stackedBodies.getTop()->appendStatement(expressionTop);
    return true;
}

bool Generation::makeIfStatement(bool hasElseBranch)
{
    // Remove the top of expression stack
    auto expressionTop = this->expressionsStack.top();
    this->expressionsStack.pop();

    // Create empty statement
    std::shared_ptr<Statement> elseBranch = std::make_shared<Body>();
    if (hasElseBranch) {
        // Get the top of statement stack
        elseBranch = this->stackedBodies.getTop();
        this->stackedBodies.popBody();
    }

    // Get the top of statement stack
    auto statementTop = this->stackedBodies.popBody();

    auto ifStatement = std::make_shared<If>();
    ifStatement->setExpression(expressionTop);
    ifStatement->setPath(0, statementTop);
    ifStatement->setPath(1, elseBranch);

    // Append while to body
    this->stackedBodies.getTop()->appendStatement(ifStatement);

    return true;
}

json Generation::serialize() const
{
    return der->to_json();
}
bool Generation::hasType(const char* name)
{
    return typeregister->hasType(name);
}

bool Generation::registerFormalParameter(sTypeDeclaration& parameter)
{
    this->localStackFrame->addVar(parameter.name, parameter.resource);
    //std::cout << "Registering parameter " << parameter.name << " "
    //            << parameter.resource->getTypeName() << std::endl;
    return true;
}

bool Generation::errorMessage(std::string message, ...)
{
    va_list parameters;
    va_start(parameters, message);
    fprintf(stderr, "[Parser]");
    vfprintf(stderr, message.c_str(), parameters);

    std::stringstream ss;
    ss << _location;
    fprintf(stderr, ", %s\n", ss.str().c_str());
    va_end(parameters);
    this->hasAnyError = true;
    return true;
}
void Generation::setDebugOn(bool state)
{
    //yydebug = 0;
    //if(state)
    //  yydebug = 1;
}

sTypeDeclaration Generation::fillTypeDeclaration(const char* type, const char* name)
{
    sTypeDeclaration result;
    result.name = name;
    result.resource = typeregister->sharedResource(type);
    return result;
}

bool Generation::initializeRule(const char* typeName)
{
    this->initializeFunction("bool");
    auto thisResource = typeregister->sharedResource(typeName);
    this->localStackFrame->addVar("this", thisResource);
    this->ruleDefinition.thisValue = this->localStackFrame->getVar("this");
    // save return condition resource
    this->ruleDefinition.conditionReturn = this->localStackFrame->getVar("_return");
    return true;
}

bool Generation::ruleProcedure(const char* typeName)
{
    this->localStackFrame->clear();
    auto thisResource = typeregister->sharedResource(typeName);
    this->localStackFrame->addVar("this", thisResource);
    this->localStackFrame->addVar("_return", typeregister->sharedResource("int"));
    return true;
}

bool Generation::makeTypeid(const char* name)
{
    std::shared_ptr<Resource> nameResource = nullptr;
    if (typeregister->hasType(name)) {
        nameResource = typeregister->sharedResource(name);
    } else {
        nameResource = this->getVariable(name);
    }

    auto result = typeregister->sharedResource("int");
    auto typexpr = std::make_shared<GetTypeId>();
    typexpr->bindInput(0, functionregister->getHandler(nameResource));
    typexpr->bindOutput(result);

    this->expressionsStack.push(typexpr);
    return true;
}

bool Generation::makeConvert(const char* name)
{
    auto expr = this->expressionsStack.top();
    this->expressionsStack.pop();

    auto convertexpr = std::make_shared<Convert>(typeregister->getTypeId(name));
    convertexpr->bindInput(0, expr);
    convertexpr->bindOutput(typeregister->sharedResource(name));

    this->expressionsStack.push(convertexpr);
    return true;
}

bool Generation::makeExplicitCast(const char* finalTypename)
{
    auto expr = this->expressionsStack.top();
    this->expressionsStack.pop();

    std::string fromType = expr->getOutput()->getTypeName();

    // is there any conversion function for these two types ?
    auto conversionFunction = functionregister->getFunc(std::string("Cast:") + finalTypename + ":" + fromType);
    if (conversionFunction == nullptr) {
        errorMessage("Invalid explicit conversion from %s to %s", fromType.c_str(), finalTypename);
    } else {
        conversionFunction->bindInput(0, expr);
        conversionFunction->bindOutput(typeregister->sharedResource(finalTypename));
        expr = conversionFunction;
    }

    this->expressionsStack.push(expr);
    return true;
}

bool Generation::makeConstructor(const char* typeName, std::vector<Argument> args)
{
    // Note: collection constructing differs in lack of type checking as it isn't neccessary
    if (strcmp(typeName, "collection") == 0) {
        auto collectionConstructor = std::make_shared<ConstructCollection>();
        assert(collectionConstructor != nullptr);
        LOG_DEBUG("Making collection constructor %s\n", typeName);
        collectionConstructor->bindOutput(this->typeregister->sharedResource("collection"));

        for (size_t i = 0; i < args.size(); i++) {
            collectionConstructor->bindInput(i, args[i]);
        }
        this->expressionsStack.push(collectionConstructor);

    } else {
        auto constructor = std::make_shared<Construct>();
        assert(constructor != nullptr);
        LOG_DEBUG("Making constructor for type %s\n", typeName);
        constructor->bindOutput(this->typeregister->sharedResource(typeName));

        auto resourceType = constructor->getOutput()->getResourceType();
        if (resourceType != ResourceType::COMPOSITE) {
            errorMessage("Creating constructor for non-composite type %s", typeName);
        } else {
            if (constructor->getCountOfComponents() != args.size()) {
                errorMessage("Invalid count of arguments for %s() constructor.", typeName);
            }
            for (size_t i = 0; i < args.size(); i++) {
                if (constructor->bindInput(i, args[i]) == false) {
                    errorMessage("Invalid parameter or type of %dth parameter in constructor of %s", i, typeName);
                }
            }
        }
        this->expressionsStack.push(constructor);
    }
    return true;
}

bool Generation::createCollectionInsert()
{
    auto element = this->expressionsStack.top();
    this->expressionsStack.pop();

    auto expr = this->expressionsStack.top();
    this->expressionsStack.pop();

    if (expr->getOutput()->getBaseId() != typeregister->getTypeId("collection")) {
        errorMessage("Expected collection, got '%s'", expr->getOutput()->getTypeName());
    }
    auto at = std::make_shared<CollectionAppend>();
    at->bindInput(0, expr);
    at->bindInput(1, element);

    this->expressionsStack.push(at);
    return true;
}

bool Generation::createCollectionAt()
{
    auto index = this->expressionsStack.top();
    this->expressionsStack.pop();

    auto expr = this->expressionsStack.top();
    this->expressionsStack.pop();
    if (expr->getOutput()->getBaseId() != typeregister->getTypeId("collection")) {
        errorMessage("Expected collection, got '%s'", expr->getOutput()->getTypeName());
    }
    auto at = std::make_shared<CollectionIndex>();
    at->bindInput(0, expr);
    at->bindInput(1, index);
    at->bindOutput(typeregister->sharedResource("any"));

    this->expressionsStack.push(at);
    return true;
}

bool Generation::createCollectionSize()
{
    auto expr = this->expressionsStack.top();
    this->expressionsStack.pop();
    if (expr->getOutput()->getBaseId() != typeregister->getTypeId("collection")) {
        errorMessage("Expected collection, got '%s'", expr->getOutput()->getTypeName());
    }
    auto at = std::make_shared<CollectionLength>();
    at->bindInput(0, expr);
    at->bindOutput(typeregister->sharedResource("int"));
    this->expressionsStack.push(at);
    return true;
}
bool Generation::createCollectionDel()
{
    auto index = this->expressionsStack.top();
    this->expressionsStack.pop();

    auto expr = this->expressionsStack.top();
    this->expressionsStack.pop();
    if (expr->getOutput()->getBaseId() != typeregister->getTypeId("collection")) {
        errorMessage("Expected collection, got '%s'", expr->getOutput()->getTypeName());
    }

    auto at = std::make_shared<CollectionRemove>();
    at->bindInput(0, expr);
    at->bindInput(1, index);

    this->expressionsStack.push(at);
    return true;
}

std::shared_ptr<Resource> Generation::getVariable(std::string name)
{
    // try local stack at first
    auto resultVariable = localStackFrame->getVar(name);
    if (resultVariable == nullptr) {
        resultVariable = globalVariables->getVar(name);
    }
    return resultVariable;
}
}
