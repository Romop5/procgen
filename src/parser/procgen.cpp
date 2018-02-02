#include "procgen.h"
#include "parser.hh"

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

	bool Generation::registerParameter(char* name, char* type)
	{

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
}
