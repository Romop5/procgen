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
 * 	PYRAMIDE(width > 5, position) :  PYRAMIDE(width*0.91,position+10), FLOOR(width,position)
 *
 */
#include "derivation.h"
#include "appender.h"
#include "obj.h"
#include <cassert>
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
	assert(inputPyramide);

	auto boolReturn = tr->sharedResource("bool");
	assert(boolReturn);

/*
 * Pseudocode logic:
 * 	if(pyramide.width > 5 )
 * 		return = true;
 * 	else
 * 		return = false;
 */

	auto getWidth = std::make_shared<CompositeGet>();
	assert(getWidth);

	auto indexWidth = tr->sharedResource("int");
	assert(indexWidth);

	*(int*) indexWidth->getData() = 0;
	getWidth->bindInput(0, fr->getHandler(inputPyramide));	
	getWidth->bindInput(1, fr->getHandler(indexWidth));	
	

	auto comparisonConstant= tr->sharedResource("float");
	*(float*) comparisonConstant->getData() = 5.0;

	auto comparisonResult= tr->sharedResource("float");
	auto greater = fr->getFunc("Greater:float");
	assert(greater);

	greater->bindInput(0, getWidth);
	greater->bindInput(1, fr->getHandler(comparisonConstant));
	greater->bindOutput(comparisonResult);

	// now we have 'greater' as expression tree which stands for
	// 	(pyramide.width > 5)
	
	// create two returns with correct value
	auto trueConstant= tr->sharedResource("bool");
	*(bool*) trueConstant->getData() = true;
	auto falseConstant= tr->sharedResource("bool");
	*(bool*) falseConstant->getData() = false;

	auto copyTrue = fr->getFunc("Copy:bool");
	assert(copyTrue);

	copyTrue->bindInput(0, fr->getHandler(trueConstant));
	copyTrue->bindOutput(boolReturn);

	auto copyFalse = fr->getFunc("Copy:bool");
	assert(copyFalse);
	copyFalse->bindInput(0, fr->getHandler(falseConstant));
	copyFalse->bindOutput(boolReturn);


	auto conversionResult= tr->sharedResource("bool");
	auto convertor = std::make_shared<FloatToBool>();
	convertor->bindInput(0, greater);
	convertor->bindOutput(conversionResult);

	auto ifbox = std::make_shared<If>();
	ifbox->setExpression(convertor);
	// on true, return true
	ifbox->setPath(0,copyTrue);
	ifbox->setPath(1,copyFalse);


	auto typePyramide = tr->getTypeId("pyramide");

	// Register function which executes ifbox, takes pyramide and returns bool
	// in 'boolReturn'
	fr->addCompositeFunction("pyramideCondition", ifbox,{inputPyramide}, boolReturn);

/*
 * Now register procedure
 * Procedure pseudocode:
 * procedure(pyramide):
 * 	Pyramide newone;
 * 	newone.width = pyramide.width*0.91;
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
	
	auto getPyramideWidth = std::make_shared<CompositeGet>();
	assert(getPyramideWidth);

	auto widthIndex = tr->sharedResource("int");
	*(int*) widthIndex->getData() = 0;

	getPyramideWidth->bindInput(0, fr->getHandler(inputProcPyramide));
	getPyramideWidth->bindInput(1, fr->getHandler(widthIndex));

	
	auto halfConstant = tr->sharedResource("float");
	*(float*) halfConstant->getData() = 0.91;
	
	auto mulResult= tr->sharedResource("float");
	auto mulBox = fr->getFunc("Mul:float");
	assert(mulBox);

	mulBox->bindInput(0, fr->getHandler(halfConstant));
	mulBox->bindInput(1, getPyramideWidth);
	mulBox->bindOutput(mulResult);

	// newone.width = pyramide.width*0.5;
	auto setPyramideWidth = std::make_shared<CompositeSet>();
	assert(setPyramideWidth);

	setPyramideWidth->bindInput(0, fr->getHandler(outputPyramide));
	setPyramideWidth->bindInput(1, fr->getHandler(widthIndex));
	setPyramideWidth->bindInput(2, mulBox);

//----------------------------------------------------------
	auto getPyramideHeigth= std::make_shared<CompositeGet>();
	assert(getPyramideHeigth);

	auto heigthIndex = tr->sharedResource("int");
	*(int*) heigthIndex->getData() = 1;

	getPyramideHeigth->bindInput(0, fr->getHandler(inputProcPyramide));
	getPyramideHeigth->bindInput(1, fr->getHandler(heigthIndex));

	
	auto twentyConstant = tr->sharedResource("float");
	*(float*) twentyConstant->getData() = 20.0;
	
	auto addResult= tr->sharedResource("float");
	auto addBox = fr->getFunc("Add:float");
	assert(addBox);

	addBox->bindInput(0, fr->getHandler(twentyConstant));
	addBox->bindInput(1, getPyramideHeigth);
	addBox->bindOutput(addResult);

	// newone.width = pyramide.width*0.5;
	auto setPyramideHeigth= std::make_shared<CompositeSet>();
	assert(setPyramideHeigth);

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

	auto setFloorWidth= std::make_shared<CompositeSet>();
	assert(setFloorWidth);

	setFloorWidth->bindInput(0, fr->getHandler(outputFloor));
	setFloorWidth->bindInput(1, fr->getHandler(widthIndex));
	setFloorWidth->bindInput(2, getPyramideWidth);

	auto setFloorHeigth= std::make_shared<CompositeSet>();
	assert(setFloorHeigth);

	setFloorHeigth->bindInput(0, fr->getHandler(outputFloor));
	setFloorHeigth->bindInput(1, fr->getHandler(heigthIndex));
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

/*
 * This implements PYRAMIDE procedure part of rule
 * 	- create a new PYRAMIDE
 * 	- creates a new primitive FLOOR
 */

void pyramide_language_second_rule(std::shared_ptr<Derivation> der, std::shared_ptr<TypeRegister> tr, std::shared_ptr<FunctionReg> fr)
{
/*
 * Now register procedure
 * Procedure pseudocode:
 * procedure(pyramide):
 * 	Pyramide newone;
 * 	newone.width = pyramide.width*0.50;
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
	
	auto getPyramideWidth = std::make_shared<CompositeGet>();
	assert(getPyramideWidth);

	auto widthIndex = tr->sharedResource("int");
	*(int*) widthIndex->getData() = 0;

	getPyramideWidth->bindInput(0, fr->getHandler(inputProcPyramide));
	getPyramideWidth->bindInput(1, fr->getHandler(widthIndex));

	
	auto halfConstant = tr->sharedResource("float");
	*(float*) halfConstant->getData() = 1.00;
	
	auto mulResult= tr->sharedResource("float");
	auto mulBox = fr->getFunc("Mul:float");
	assert(mulBox);

	mulBox->bindInput(0, fr->getHandler(halfConstant));
	mulBox->bindInput(1, getPyramideWidth);
	mulBox->bindOutput(mulResult);

	// newone.width = pyramide.width*0.5;
	auto setPyramideWidth = std::make_shared<CompositeSet>();
	assert(setPyramideWidth);

	setPyramideWidth->bindInput(0, fr->getHandler(outputPyramide));
	setPyramideWidth->bindInput(1, fr->getHandler(widthIndex));
	setPyramideWidth->bindInput(2, mulBox);

//----------------------------------------------------------
	auto getPyramideHeigth= std::make_shared<CompositeGet>();
	assert(getPyramideHeigth);

	auto heigthIndex = tr->sharedResource("int");
	*(int*) heigthIndex->getData() = 1;

	getPyramideHeigth->bindInput(0, fr->getHandler(inputProcPyramide));
	getPyramideHeigth->bindInput(1, fr->getHandler(heigthIndex));

	
	auto twentyConstant = tr->sharedResource("float");
	*(float*) twentyConstant->getData() = 20.0;
	
	auto addResult= tr->sharedResource("float");
	auto addBox = fr->getFunc("Add:float");
	assert(addBox);

	addBox->bindInput(0, fr->getHandler(twentyConstant));
	addBox->bindInput(1, getPyramideHeigth);
	addBox->bindOutput(addResult);

	// newone.width = pyramide.width*0.5;
	auto setPyramideHeigth= std::make_shared<CompositeSet>();
	assert(setPyramideHeigth);

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

	auto setFloorWidth= std::make_shared<CompositeSet>();
	assert(setFloorWidth);

	setFloorWidth->bindInput(0, fr->getHandler(outputFloor));
	setFloorWidth->bindInput(1, fr->getHandler(widthIndex));
	setFloorWidth->bindInput(2, getPyramideWidth);

	auto setFloorHeigth= std::make_shared<CompositeSet>();
	assert(setFloorHeigth);

	setFloorHeigth->bindInput(0, fr->getHandler(outputFloor));
	setFloorHeigth->bindInput(1, fr->getHandler(heigthIndex));
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

	fr->addCompositeFunction("pyramideProcedureSecond", body,{inputProcPyramide}, nullptr);

}

// Print out result
void inspectResultSymbols(TypeId floor, const std::vector<std::shared_ptr<Resource>>& symbols)
{

	Object o;
	std::cout << ">>>>>>> RESULTING OBJECT >>>>>>\n";
	for(auto &x: symbols)
	{
		if(x->getBaseId() == floor)
		{
			auto floorObject = std::dynamic_pointer_cast<CompositeResource>(x);
			float width = *(float*) floorObject->getComponent(0)->getData();
			float heigth = *(float*) floorObject->getComponent(1)->getData();
			std::cout << "Floor object: " << width << " " << heigth << std::endl;
			o.addPrimitive(glm::vec3(0.0f,0.0f,heigth),glm::vec3(width,width,20.0f));
		} else {
			auto floorObject = std::dynamic_pointer_cast<CompositeResource>(x);
			float width = *(float*) floorObject->getComponent(0)->getData();
			float heigth = *(float*) floorObject->getComponent(1)->getData();
			std::cout << "Pyramide object: " << width << " " << heigth << std::endl;
			
		}
	}

	o.dumpVertices("pyramide.obj");

}


int main(int argc, char **argv)
{
	std::cout << "Generate pyramide\n" << std::endl;
	if(argc < 2)
	{	
		std::cout << "FORMAT: <iteration-count>\n";
		return 1;
	}
	unsigned int iterationCount = atoi(argv[1]);
	srandom(time(NULL));
	
	auto tr = std::make_shared<TypeRegister>();
	auto fr = std::make_shared<FunctionReg>(tr);
	
	registerStandardTypes(tr.get());
	registerStandardFunctions(fr.get());

	auto derivation = std::make_shared<Derivation>(tr,fr);


	// Add rule
	
	pyramide_language(derivation,tr,fr);
	pyramide_language_second_rule(derivation,tr,fr);

	auto pyramideCond= fr->getFunc("pyramideCondition");
	auto pyramideProc= fr->getFunc("pyramideProcedure");
	auto pyramideProcSecond= fr->getFunc("pyramideProcedureSecond");


	derivation->addRule(tr->getTypeId("pyramide"), pyramideCond,pyramideProc);
	derivation->addRule(tr->getTypeId("pyramide"), pyramideCond,pyramideProcSecond);

/*
 * Starting symbol
 * 	pyramide(50, 0)
 */

	auto startPyramide = std::dynamic_pointer_cast<CompositeResource>(tr->sharedResource("pyramide"));
	assert(startPyramide);

	*(float*) startPyramide->getComponent(0)->getData() = 1000;
	*(float*) startPyramide->getComponent(1)->getData() = 0;

	std::vector<std::shared_ptr<Resource>> symbols = {startPyramide};

	derivation->setStartSymbols(symbols);

/*
 * Run generation
 */
	derivation->generate(iterationCount);
	
	auto syms= derivation->getCurrentSymbolList();

	inspectResultSymbols(tr->getTypeId("floor"), syms);


	return 0;
}
