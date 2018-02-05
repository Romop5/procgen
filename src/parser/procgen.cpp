#include "procgen.h"
#include "parser.hh"
#include <sstream>
#include "appender.h"

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
	}
	
	bool Generation::parseFile(const std::string& file)
	{
		yydebug = 1;
		yyin = fopen(file.c_str(),"r");
		if(yyin == NULL)
			error(1,0,"Failed to open file ...");
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
            error(0,0,"Missing init() function");
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
            error(0,0,"Failed to create %s resource. \n",type);
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
		auto typeResource = typeregister->sharedResource(type);
		functionregister->addCompositeFunction(
				ruleName.str(), procedure,{typeResource}, nullptr);

		// Get condition
		//
		//auto boolResult= typeregister->sharedResource("bool");
		//TODO
        auto boolResult = this->localStackFrame->getVar("_return");
		auto condition= this->stackedBodies.top();
		this->stackedBodies.pop();

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
			std::cout << "registerStruct" << x.mType << std::endl;
			std::string str = x.mType;
			types.push_back(typeregister->getTypeId(str));
			names.push_back(x.mName);
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
			error(0,0,"Unk type %s\n", type);
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
		// Create output resource
		auto resource = typeregister->sharedResource(type);
		if(resource == nullptr)
		{
			error(0,0,"Unknown type: %s\n", type);
		}

		// Get the top of statement stack
		auto statementTop = this->stackedBodies.top();
		this->stackedBodies.pop();

		// Create a vector of input parameters
		std::vector<std::shared_ptr<Resource>> inputResources;
	
		// type list
		for(auto &typeDesc: typeList)
			inputResources.push_back(typeregister->sharedResource(typeDesc.mType));

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
			error(0,0,"Semantic error: types don't match");
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
			default:
				  error(0,0,"Undefined operation");
		}

		// Create operation function

		std::cout << type+":"+operationName<< " je kunda" << std::endl;
		auto operationBox = functionregister->getFunc(operationName+":"+type);
		if(!operationBox)
		{
			error(0,0,"Failed to create an instance for operation");
		}

		auto tmpResult = typeregister->sharedResource(a);
		operationBox->bindInput(0,first);
		operationBox->bindInput(1,second);
		operationBox->bindOutput(tmpResult);

		this->expressionsStack.push(operationBox);
		return operationBox;	
	}

    void Generation::createLiteralBool(bool value)
	{
		auto val = typeregister->sharedResource("bool");
		if(!val)
			error(0,0,"Unexpected error while creating bool resource");

		*(bool*) val->getData() = value;
		this->expressionsStack.push(functionregister->getHandler(val));
	}


	void Generation::createLiteralInteger(int value)
	{
		auto val = typeregister->sharedResource("int");
		if(!val)
			error(0,0,"Unexpected error while creating int resource");

		*(int*) val->getData() = value;
		this->expressionsStack.push(functionregister->getHandler(val));
	}

	void Generation::createLiteralFloat(float value)
	{
		auto val = typeregister->sharedResource("float");
		if(!val)
			error(0,0,"Unexpected error while creating flaot resource");

		*(float*) val->getData() = value;
		this->expressionsStack.push(functionregister->getHandler(val));
	}

    bool Generation::createLiteralFromVariable(char* name)
    {
        auto res = localStackFrame->getVar(name);
        if(res == nullptr)
        {
            error(0,0,"Undefined variable %s\n", name);
            return false;
        }
        this->expressionsStack.push(functionregister->getHandler(res));
        return true;
    }

	bool Generation::createFunctionCall(const char* functionName)
	{
		auto functionPointer = functionregister->getFunc(functionName);
		if(functionPointer == nullptr)
		{
			// if functionName doesn't exist
			error(0,0,"Failed to get function '%s'", functionName);
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

	bool Generation::registerLocalVariable(const char* type, const char* name)
	{
		auto resource = typeregister->sharedResource(type);
		if(resource == nullptr)
			return false;
		return this->localStackFrame->addVar(name,resource);
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
            auto typeName = typeregister->getTypeName(resource->getBaseId());
            auto function = functionregister->getFunc(std::string("Copy")+":"+typeName);

            auto expr = this->expressionsStack.top(); 
            this->expressionsStack.pop();

            // Bind return box with resources
            function->bindInput(0,expr);
            function->bindOutput(resource);
    
            box->bindInput(function);
        }
		// Register return
		this->stackedBodies.top()->appendStatement(box); 

    }
	bool Generation::makeAssignment(const char* name)
	{
		// get resourse
		auto resource = localStackFrame->getVar(name);
		auto typeName = typeregister->getTypeName(resource->getBaseId());
		auto function = functionregister->getFunc(std::string("Copy")+":"+typeName);
		if(function == nullptr)
		{
			error(0,0,"Failed to create assignment for type %s, variable %s\n",
					typeName.c_str(), name);
		}

        // Get expression
		auto expressionTop= this->expressionsStack.top();
		this->expressionsStack.pop();

        // resource = expressionTop
        function->bindInput(0,expressionTop);
        function->bindOutput(resource);

		// Register 
		this->stackedBodies.top()->appendStatement(function);
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

		std::shared_ptr<Statement> elseBranch = nullptr;
		if(hasElseBranch)
		{
			// Get the top of statement stack
			auto elseBranch= this->stackedBodies.top();
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

    json11::Json Generation::serialize() const
    {
        return der->to_json();
    }    
}
