#include "procgen.h"
#include "parser.hh"

extern FILE* yyin;
namespace ProcGen {

	Generation::Generation()
	{
		typeregister = std::make_shared<TypeRegister>();
		functionregister = std::make_shared<FunctionReg>(typeregister);
		der = std::make_shared<Derivation>(typeregister,functionregister);
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


	std::shared_ptr<Function> Generation::createExpressionOperation(char operation, 
			std::shared_ptr<Function> first, std::shared_ptr<Function> second)
	{
		// TODO: produce common type

		TypeId a = 0;
		if(first->getOutput() != nullptr)
			a = first->getOutput().getBaseId();

		TypeId b = 0;
		if(second->getOutput() != nullptr)
			a = second->getOutput().getBaseId();

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
			case "+": operationName = "Add"; break;
			case "-": operationName = "Sub"; break;
			case "/": operationName = "Div"; break;
			case "*": operationName = "Mul"; break;
			default:
				  error(0,0,"Undefined operation");
		}

		// Create operation function

		auto operationBox = functionregister->getFunc(type+":"+operationName);
		if(!operationBox)
		{
			error(0,0,"Failed to create an instance for operation");
		}

		auto tmpResult = typeregister->sharedResource(a);
		operationBox->bindInput(0,first);
		operationBox->bindInput(1,second);
		operationBox->bindOutput(tmpResult);
		return operationBox;	
	}

}
