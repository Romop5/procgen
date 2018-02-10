#include "procgen.h"
#include "parser.hh"
#include <sstream>
#include "appender.h"

#include <cstdarg>

extern FILE* yyin;
namespace ProcGen {

	Generation::Generation()
	{
		typeregister = std::make_shared<TypeRegister>();
		functionregister = std::make_shared<FunctionReg>(typeregister);
		localStackFrame = std::make_shared<VariableReg>(typeregister);	
		globalVariables = std::make_shared<VariableReg>(typeregister);	
		der = std::make_shared<Derivation>(typeregister,functionregister);

		registerStandardTypes(typeregister.get());
		registerStandardFunctions(functionregister.get());

        // Add derivation standards
        auto derivation = this->der;
		functionregister->addFunction("appendSymbol",
             [derivation]()->std::shared_ptr<Function>{return std::static_pointer_cast<Function>(std::make_shared<AppendSymbol>(derivation));});

        flagIsParsed = true;
        hasAnyError = false;;
	}
	
	bool Generation::parseFile(const std::string& file)
	{
		////yydebug = 1;/
		yyin = fopen(file.c_str(),"r");
		if(yyin == NULL)
			errorMessage("Failed to open file ...");
		do {
			yyparse(this);
		} while (!feof(yyin));
		return true;
	}

    bool Generation::runInit()
    {
        auto initFunction = functionregister->getFunc("init");
        if(initFunction == nullptr)
        {
            errorMessage("Missing init() function");
            return false;
        }
        RunStatus rs;
        (*initFunction)(rs);
        std::cout << "Initialized...\n";
        return true;
    }
    
    bool Generation::run(int maximumSteps)
    {
        der->generate(maximumSteps);
        std::cout << "Done...\n";
    }

    
    bool Generation::initializeFunction(char* type)
    {
        this->localStackFrame->clear();
        auto returnResource = typeregister->sharedResource(type);
        if(returnResource == nullptr)
        {
            errorMessage("Failed to create %s resource. \n",type);
            return false;  
        }
        // Add special local variable for returning function value
        this->localStackFrame->addVar("_return", returnResource);
        return true;
    }

	bool Generation::registerRule(char* name,char* type)
	{
		static int ruleID = 0;
		ruleID++;

		// Get procedure
		auto procedure= this->stackedBodies.top();
		this->stackedBodies.pop();

		std::stringstream ruleName;
	       	ruleName << "rule" << ruleID;
		//auto typeResource = typeregister->sharedResource(type);
		auto typeResource = localStackFrame->getVar("this"); 
		functionregister->addCompositeFunction(
				ruleName.str(), procedure,{typeResource}, nullptr);

		// Get condition
		//
		//auto boolResult= typeregister->sharedResource("bool");
		//TODO
        auto boolResult = ruleDefinition.conditionReturn;
		auto condition= this->stackedBodies.top();
		this->stackedBodies.pop();

        typeResource = this->ruleDefinition.conditionReturn;
		std::stringstream conditionName;
	       	conditionName	<< "condition" << ruleID;
		functionregister->addCompositeFunction(
				conditionName.str(), condition,{typeResource}, boolResult);

		TypeId typeId = typeregister->getTypeId(type);
		return der->addRule(typeId,functionregister->getFunc(conditionName.str()),
					functionregister->getFunc(ruleName.str()));
	}
	bool Generation::registerAlias(char* alias, char* aliasedType)
	{
		return typeregister->addAlias(alias,aliasedType);			
	}


	bool Generation::registerStruct(char* name, std::vector<sTypeDeclaration>& typelist)
	{
		std::vector<TypeId> 	types;
		std::vector<std::string> names;
		for(auto &x: typelist)
		{
			//TODO: check if type exists
            std::cout << "registerStruct" << std::endl;
            types.push_back(x.resource->getBaseId());
			names.push_back(x.name);
		}
		// Clear types
		typelist.clear();
		// TODO: add error message 
		return typeregister->addCompositeWithNames(name, types,names);
	}

	bool Generation::registerParameter(char* name, char* type,bool hasLiteral)
	{
		// Get type
		auto resource = typeregister->sharedResource(type);
		if(resource == nullptr)
		{
			errorMessage("Unk type %s\n", type);
		}

		globalVariables->addVar(name,resource);

		// TODO: check types and do implicit converion if neccesary
		// TODO: use hasLiteral and literal
		auto literal = this->expressionsStack.top();
		if(hasLiteral)
			this->expressionsStack.pop();

	}

	bool Generation::registerFunction(char* type, char* name)
	{
        auto resource = localStackFrame->getVar("_return");
		if(resource == nullptr)
		{
			errorMessage("Unknown type: %s\n", type);
		}

		// Get the top of statement stack
		auto statementTop = this->stackedBodies.top();
		this->stackedBodies.pop();

		// Create a vector of input parameters
		std::vector<std::shared_ptr<Resource>> inputResources;
	
		// type list
        std::cout << "Registering function " << name << "with num. of params:"
            << typeList.size() << "\n";
		for(auto &typeDesc: typeList)
        {
			inputResources.push_back(typeDesc.resource);
            std::cout << "Parameter: " << typeDesc.name << " with type: "
                << typeregister->getTypeName(typeDesc.resource->getBaseId()) 
                << std::endl;
        }

        typeList.clear();

		return functionregister->addCompositeFunction(name, 
			statementTop, inputResources,resource);	
	}


	std::shared_ptr<Function> Generation::createExpressionOperation(char operation)
	{
		// TODO: produce common type
		std::shared_ptr<Function> first= this->expressionsStack.top();
		this->expressionsStack.pop();
		std::shared_ptr<Function> second = this->expressionsStack.top();
		this->expressionsStack.pop();

		TypeId a = 0;
		if(first->getOutput() != nullptr)
			a = first->getOutput()->getBaseId();

		TypeId b = 0;
		if(second->getOutput() != nullptr)
			b = second->getOutput()->getBaseId();

		// TODO: determien if types are compatible
		// and which type we should use
		// and arrange implicit conversion if neccessary
		
		if(a != b)
		{
			//TODO
			errorMessage("Semantic error: types don't match. Types:\n\
                    %s, %s\n",
                     typeregister->getTypeName(a).c_str(),
                     typeregister->getTypeName(b).c_str());
		}

		// TODO: watch out for aliases
		
		std::string type = typeregister->getTypeName(a);
		std::string operationName = "";
		switch(operation)
		{
			case '+': operationName = "Add"; break;
			case '-': operationName = "Sub"; break;
			case '/': operationName = "Div"; break;
			case '*': operationName = "Mul"; break;
			case '>': operationName = "Greater"; break;
			case '<': operationName = "Less"; break;
			case '=': operationName = "Eq"; break;
			case '!': operationName = "NotEq"; break;
			default:
				  errorMessage("Undefined operation");
		}

		// Create operation function

		std::cout << type+":"+operationName<< " je kunda" << std::endl;
		auto operationBox = functionregister->getFunc(operationName+":"+type);
		if(!operationBox)
		{
			errorMessage("Failed to create an instance for operation");
		}

		auto tmpResult = typeregister->sharedResource(a);
		operationBox->bindInput(0,second);
		operationBox->bindInput(1,first);
		operationBox->bindOutput(tmpResult);

		this->expressionsStack.push(operationBox);
		return operationBox;	
	}


    void Generation::createLiteralBool(bool value)
	{
		auto val = typeregister->sharedResource("bool");
		if(!val)
			errorMessage("Unexpected error while creating bool resource");

		*(bool*) val->getData() = value;
		this->expressionsStack.push(functionregister->getHandler(val));
	}


	void Generation::createLiteralInteger(int value)
	{
		auto val = typeregister->sharedResource("int");
		if(!val)
			errorMessage("Unexpected error while creating int resource");

		*(int*) val->getData() = value;
		this->expressionsStack.push(functionregister->getHandler(val));
	}

	void Generation::createLiteralFloat(float value)
	{
		auto val = typeregister->sharedResource("float");
		if(!val)
			errorMessage("Unexpected error while creating flaot resource");

		*(float*) val->getData() = value;
		this->expressionsStack.push(functionregister->getHandler(val));
	}

    bool Generation::createLiteralFromVariable(char* name)
    {
        auto res = localStackFrame->getVar(name);
        if(res == nullptr)
        {
            errorMessage("Undefined variable %s\n", name);
            return false;
        }
        std::cout << "Pushing variable " << name << " with type " <<
                typeregister->getTypeName(res->getBaseId()) << std::endl;
        this->expressionsStack.push(functionregister->getHandler(res));
        return true;
    }

    bool Generation::createStructuredLiteral(char* member)
    {
        auto compositeFunction = this->expressionsStack.top();
        this->expressionsStack.pop();

        auto resource = compositeFunction->getOutput();
        if(resource == nullptr)
        {
            errorMessage("Failed to get resource");
        }
        if(resource->getResourceType() != ResourceType::COMPOSITE)
        {
            errorMessage("Isn't composite at all.");
        }
        auto compositeResource = 
                std::dynamic_pointer_cast<CompositeResource>(resource);
        
        auto structureName = typeregister->getTypeName(compositeResource->getBaseId());
        auto position = compositeResource->getComponentPosition(member);
        if(position == -1)
        {
            errorMessage("'%s' doesn't contain name '%s'",structureName.c_str(),member);
        }

        TypeId memberType = compositeResource->getComponentType(position);

        // set index
        auto indexResoure = typeregister->sharedResource("int");
        *(int*) indexResoure->getData() = position;
        // create accessor
        auto getter = std::make_shared<CompositeGet>();
        getter->bindInput(0, compositeFunction);
        getter->bindInput(1, functionregister->getHandler(indexResoure));

        // This is actually useless for interpreter as it will replace
        // output resource when operator() is run
        // However, it's neccessary for parser to get base structure metadata
        auto outputResource = typeregister->sharedResource(memberType);
        getter->bindOutput(outputResource);


        std::cout << "Pushing structuremember '" << member << "' with type " <<
                typeregister->getTypeName(outputResource->getBaseId()) << std::endl;
        this->expressionsStack.push(getter);
    }

	bool Generation::createFunctionCall(const char* functionName)
	{
		auto functionPointer = functionregister->getFunc(functionName);
		if(functionPointer == nullptr)
		{
			// if functionName doesn't exist
			errorMessage("Failed to get function '%s'", functionName);
			return false;
		}

		// For all given parameters 
		for(unsigned param = 0; param < argumentVector.size(); param++)
		{
			// TODO: semantic control / implicit conversion
			functionPointer->bindInput(param,argumentVector[param]);
		}

		// Clear arguments 
		this->argumentVector.clear();

		// Push new expression(function call) to expression stack
		this->expressionsStack.push(functionPointer);
		return true;
	}

	bool Generation::createArgument()
	{
		std::shared_ptr<Function> expr= this->expressionsStack.top();
		this->expressionsStack.pop();
		this->argumentVector.push_back(expr);
	}

    bool Generation::registerLocalVariable(const char* type, const char* name,bool hasExp)
	{
		auto resource = typeregister->sharedResource(type);
		if(resource == nullptr)
        {
            errorMessage("Invalid type %s\n", type);
            return false;
        }
		this->localStackFrame->addVar(name,resource);

        // if has = expression
        if(hasExp)
        {
            // create expression with name
            auto swap = this->expressionsStack.top();
            this->expressionsStack.pop();
            // A = B, B must be at top, that's why we need to push it later
            this->expressionsStack.push(functionregister->getHandler(resource));
            this->expressionsStack.push(swap);
            return this->makeAssignment(name);
        }
        return true;
	}

	bool Generation::pushBody()
	{
		this->stackedBodies.push(std::make_shared<Body>());
	}

	std::shared_ptr<Body> Generation::popBody()
	{
		auto top = this->stackedBodies.top();
		this->stackedBodies.pop();
		return top;
	}

    bool Generation::makeReturn(bool hasExpression)
    {
        auto box = std::make_shared<Return>();
        if(hasExpression)
        {

            // get resourse
            auto resource = localStackFrame->getVar("_return");
      /*      auto typeName = typeregister->getTypeName(resource->getBaseId());
            auto function = functionregister->getFunc(std::string("Copy")+":"+typeName);


            // Bind return box with resources
            function->bindInput(0,expr);
            function->bindOutput(resource);
*/

            auto expr = this->expressionsStack.top(); 
            this->expressionsStack.pop();

            auto genericCopy = std::make_shared<GenericCopy>();
            genericCopy->bindInput(0,functionregister->getHandler(resource));
            genericCopy->bindInput(1,expr);
            
    
            box->bindInput(genericCopy);
        }
		// Register return
		this->stackedBodies.top()->appendStatement(box); 

    }
	bool Generation::makeAssignment(const char* name)
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
		auto expressionTop= this->expressionsStack.top();
		this->expressionsStack.pop();

        // Get expression
		auto assignedResource = this->expressionsStack.top();
		this->expressionsStack.pop();

        auto genericCopy= std::make_shared<GenericCopy>();
        genericCopy->bindInput(0, assignedResource);
        genericCopy->bindInput(1, expressionTop);
		// Register 
		this->stackedBodies.top()->appendStatement(genericCopy);
	}

	bool Generation::makeWhile()
	{
		// Remove the top of expression stack
		auto expressionTop= this->expressionsStack.top();
		this->expressionsStack.pop();

		// Get the top of statement stack
		auto statementTop = this->stackedBodies.top();
		this->stackedBodies.pop();

		auto whileStatement = std::make_shared<While>();
		whileStatement->bindCondition(expressionTop);
		whileStatement->bindStatement(statementTop);

		// Append while to body
		this->stackedBodies.top()->appendStatement(whileStatement);
	}

    bool Generation::makeCallStatement()
    {
		// Get expression
		auto expressionTop= this->expressionsStack.top();
		this->expressionsStack.pop();

        this->stackedBodies.top()->appendStatement(expressionTop);
    }

	bool Generation::makeIfStatement(bool hasElseBranch)
	{
		// Remove the top of expression stack
		auto expressionTop= this->expressionsStack.top();
		this->expressionsStack.pop();

		// Create empty statement 
		std::shared_ptr<Statement> elseBranch = std::make_shared<Body>();
		if(hasElseBranch)
		{
			// Get the top of statement stack
			elseBranch= this->stackedBodies.top();
			this->stackedBodies.pop();
		}

		// Get the top of statement stack
		auto statementTop = this->stackedBodies.top();
		this->stackedBodies.pop();

		auto ifStatement = std::make_shared<If>();
		ifStatement->setExpression(expressionTop);
		ifStatement->setPath(0,statementTop);
		ifStatement->setPath(1,elseBranch);

		// Append while to body
		this->stackedBodies.top()->appendStatement(ifStatement);
	
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
        this->localStackFrame->addVar(parameter.name,parameter.resource);
        std::cout << "Registering parameter " << parameter.name << " "
                    << typeregister->getTypeName(parameter.resource->getBaseId()) << std::endl;
        return true;
    }

    bool Generation::errorMessage(const char* message, ...)
    {
        va_list parameters;
        va_start(parameters,message); 
        fprintf(stderr,"[Parser]");
        vfprintf(stderr, message, parameters);
        fprintf(stderr,"Position: %d.%d\n", yylloc.first_line,yylloc.first_column);
        va_end(parameters);
        this->hasAnyError = true;
    }
    void Generation::setDebugOn(bool state)
    {
        yydebug = 0;
        if(state)
            yydebug = 1;
    }

    sTypeDeclaration Generation::fillTypeDeclaration(char* type, char* name)
    {
        sTypeDeclaration result;
        result.name = name;
        result.resource = typeregister->sharedResource(type);
        return result;
    }

    bool Generation::initializeRule(char* typeName)
    {
        this->initializeFunction("bool");
        auto thisResource = typeregister->sharedResource(typeName);
        this->localStackFrame->addVar("this", thisResource);        
        // save return condition resource
        this->ruleDefinition.conditionReturn = 
                this->localStackFrame->getVar("_return");
    }

    bool Generation::ruleProcedure(char* typeName)
    {
        this->localStackFrame->clear();
        auto thisResource = typeregister->sharedResource(typeName);
        this->localStackFrame->addVar("this", thisResource);        

    }

	bool Generation::makeTypeid(char* name)
	{
		std::shared_ptr<Resource> nameResource = nullptr;
		if(typeregister->hasType(name))
		{
			nameResource = typeregister->sharedResource(name);
		} else {
			nameResource = localStackFrame->getVar(name);
		}

		auto result = typeregister->sharedResource("int");
		auto typexpr = std::make_shared<GetTypeId>();
		typexpr->bindInput(0,functionregister->getHandler(nameResource));
		typexpr->bindOutput(result);

		this->expressionsStack.push(typexpr);
	}

	bool Generation::makeConvert(char* name)
	{
		auto expr = this->expressionsStack.top();
		this->expressionsStack.pop();

		auto convertexpr = std::make_shared<Convert>(typeregister->getTypeId(name));
		convertexpr->bindInput(0,expr);
		convertexpr->bindOutput(typeregister->sharedResource(name));

		this->expressionsStack.push(convertexpr);
	}
}

