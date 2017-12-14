/*
 * 2017/12/15 Roman Dobias
 *
 * Pyramide generator 
 *
 * PYRAMIDE:
 * 	float startingWidth;
 * 	float starting position;
 * FLOOR:
 * 	float width;
 * 	float positionY;
 *
 * Note: FLOOR is cuboid (temporary a terminating symbol)
 * Note 2: this gonna be replaced with quad as terminating symbol THE MOMENT the language parsing is ready
 * Grammar:
 * 	PYRAMIDE(width > 100, position) :  PYRAMIDE(width*0.5,position+10), FLOOR(width,position)
 *
 */
#include "derivation.h"
#include "appender.h"

/*
 * This implements PYRAMIDE procedure part of rule
 * 	- create a new PYRAMIDE
 * 	- creates a new primitive FLOOR
 */

void pyramide_language(std::shared_ptr<Derivation> der, std::shared_ptr<TypeRegister> tr, std::shared_ptr<FunctionReg> fr)
{

/*
 * Register two new types:
 * 	- PYRAMIDE as (float, float)
 * 	- FLOOR as (float, float)
 */

	auto floatType = tr->getTypeId("float");
	auto floatVector = {floatType, floatType};

	// Register 'pyramide' (float,float)
	tr->addComposite("pyramide", floatVector);
	// Register 'floor' (float,float)
	tr->addComposite("floor", floatVector);

	
	// Create predicate function for 'pyramide rule'
	
	auto inputPyramide = tr->sharedResource("pyramide");
	auto boolReturn = tr->sharedResource("bool");

/*
 * Pseudocode logic:
 * 	if(pyramide.width > 100 )
 * 		return = true;
 * 	else
 * 		return = false;
 */

	auto getWidth = std::make_shared<CompositeGet>();
	auto indexWidth = tr->sharedResource("int");
	*(int*) indexWidth->getData() = 0;
	getWidth->bindInput(0, fr->getHandler(inputPyramide));	
	getWidth->bindInput(1, fr->getHandler(indexWidth));	
	

	auto comparisonConstant= tr->sharedResource("int");
	*(int*) comparisonConstant->getData() = 100;

	auto comparisonResult= tr->sharedResource("bool");
	auto greater = fr->getFunc("int:Greated");
	greater->bindInput(0, getWidth);
	greater->bindInput(1, fr->getHandler(comparisonConstant));
	greater->bindOutput(comparisonResult);

	// now we have 'greater' as expression tree which stands for
	// 	(pyramide.width > 100)
	
	// create two returns with correct value
	auto trueConstant= tr->sharedResource("bool");
	*(bool*) trueConstant->getData() = true;
	auto falseConstant= tr->sharedResource("bool");
	*(bool*) falseConstant->getData() = false;

	auto copyTrue = fr->getFunc("bool:Copy");
	copyTrue->bindInput(0, fr->getHandler(trueConstant));
	copyTrue->bindOutput(boolReturn);

	auto copyFalse = fr->getFunc("bool:Copy");
	copyFalse->bindInput(0, fr->getHandler(trueConstant));
	copyTrue->bindOutput(boolReturn);


	auto ifbox = std::make_shared<If>();
	ifbox->setExpression(greater);
	// on true, return true
	ifbox->setPath(0,copyTrue);
	ifbox->setPath(1,copyTrue);


	auto typePyramide = tr->getTypeId("pyramide");

	// Register function which executes ifbox, takes pyramide and returns bool
	// in 'boolReturn'
	fr->addCompositeFunction("pyramideCondition", ifbox,{inputPyramide}, boolReturn);

/*
 * Now register procedure
 * Procedure pseudocode:
 * procedure(pyramide):
 * 	Pyramide newone;
 * 	newone.width = pyramide.width*0.5;
 * 	newone.heigth = pyramide.height+20;
 * 	appendSymbol(newone);
 *
 * 	Floor newflor;
 * 	flor.width = pyramide.width;	
 * 	flor.heigth = pyramide.heigth;	
 * 	appendSymbol(flor);
 */
		

	auto inputProcPyramide = tr->sharedResource("pyramide");		
	auto outputPyramide = tr->sharedResource("pyramide");		
	
	auto getPyramideWidth = fr->getFunc("get");
	auto widthIndex = tr->sharedResource("int");
	*(int*) widthIndex->getData() = 0;

	getPyramideWidth->bindInput(0, fr->getHandler(inputProcPyramide));
	getPyramideWidth->bindInput(1, fr->getHandler(widthIndex));

	
	auto halfConstant = tr->sharedResource("float");
	*(float*) halfConstant->getData() = 0.5;
	
	auto mulResult= tr->sharedResource("float");
	auto mulBox = fr->getFunc("float:Mul");
	mulBox->bindInput(0, fr->getHandler(halfConstant));
	mulBox->bindInput(1, getPyramideWidth);
	mulBox->bindOutput(mulResult);

	// newone.width = pyramide.width*0.5;
	auto setPyramideWidth = fr->getFunc("set");
	setPyramideWidth->bindInput(0, fr->getHandler(outputPyramide));
	setPyramideWidth->bindInput(1, fr->getHandler(widthIndex));
	setPyramideWidth->bindInput(2, mulBox);

//----------------------------------------------------------
	auto getPyramideHeigth= fr->getFunc("get");
	auto heigthIndex = tr->sharedResource("int");
	*(int*) heigthIndex->getData() = 1;

	getPyramideHeigth->bindInput(0, fr->getHandler(inputProcPyramide));
	getPyramideHeigth->bindInput(1, fr->getHandler(heigthIndex));

	
	auto twentyConstant = tr->sharedResource("float");
	*(float*) twentyConstant->getData() = 20.0;
	
	auto addResult= tr->sharedResource("float");
	auto addBox = fr->getFunc("float:Add");
	addBox->bindInput(0, fr->getHandler(twentyConstant));
	addBox->bindInput(1, getPyramideHeigth);
	addBox->bindOutput(addResult);

	// newone.width = pyramide.width*0.5;
	auto setPyramideHeigth= fr->getFunc("set");
	setPyramideHeigth->bindInput(0, fr->getHandler(outputPyramide));
	setPyramideHeigth->bindInput(1, fr->getHandler(heigthIndex));
	setPyramideHeigth->bindInput(2, addBox);

	// append new symbol
	auto appendPyramide = std::make_shared<AppendSymbol>(der);
	appendPyramide->bindInput(0, fr->getHandler(outputPyramide));

/*
 * Now lets create new floor
 */

	auto outputFloor = tr->sharedResource("floor");

	auto setFloorWidth= fr->getFunc("set");
	setFloorWidth->bindInput(0, fr->getHandler(outputFloor));
	setFloorWidth->bindInput(1, fr->getHandler(widthIndex));
	setFloorWidth->bindInput(2, getPyramideWidth);

	auto setFloorHeigth= fr->getFunc("set");
	setFloorHeigth->bindInput(0, fr->getHandler(outputFloor));
	setFloorHeigth->bindInput(1, fr->getHandler(widthIndex));
	setFloorHeigth->bindInput(2, getPyramideHeigth);

	// append new symbol
	auto appendFloor= std::make_shared<AppendSymbol>(der);
	appendFloor->bindInput(0, fr->getHandler(outputFloor));

	auto body = std::make_shared<Body>();

/*
 * Finally, put it together & register as func
 */
	body->append(setPyramideWidth);
	body->append(setPyramideHeigth);
	body->append(appendPyramide);
	body->append(setFloorWidth);
	body->append(setFloorHeigth);
	body->append(appendFloor);

	fr->addCompositeFunction("pyramideProcedure", body,{inputProcPyramide}, nullptr);

}

int main(int argc, char **argv)
{
	std::cerr << "Produces Nth generation of algae string" << std::endl;
	if(argc < 2)
	{
		std::cout << "Format: <iteration-count>" << std::endl;
	}
	srandom(time(NULL));
	
	auto tr = std::make_shared<TypeRegister>();
	auto fr = std::make_shared<FunctionReg>(tr);
	
	registerStandardTypes(tr.get());
	registerStandardFunctions(fr.get());

	auto derivation = std::make_shared<Derivation>(tr,fr);


	/* L-sys grammar */

	// int -> int float 
	// float -> int
	
	auto intValue = tr->sharedResource("int");
	auto floatValue  = tr->sharedResource("float");

	std::vector<std::shared_ptr<Resource>> symbols = {intValue};

	derivation->setStartSymbols(symbols);

	// Add rule
	
	pyramide_language(derivation,tr,fr);

	auto pyramideCond= fr->getFunc("pyramideCondition");
	auto pyramideProc= fr->getFunc("pyramideProcedure");


	derivation->addRule(tr->getTypeId("pyramide"), pyramideCond,pyramideProc);
	derivation->generate(atoi(argv[1]));
	
	auto syms= derivation->getCurrentSymbolList();


	return 0;
}
