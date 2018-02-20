/*
 * 2017/12/16 Roman Dobias
 *
 * 2D cantor set fractal
 *
 * CANTOR:
 * 	float x
 * 	float y
 * 	float z
 * 	float measure 
 *
 * RULES:
 * 	rule 1 (cantor)(always):
 * 		append(PRIMITIVE(
 * 			cantor.x+cantor.measure*2/3,
 * 			cantor.y+cantor.measure*2/3,
 * 			cantor.z+cantor.measure*2/3,
 * 			cantor.measure/3
 * 			)
 *
 * 		append(PRIMITIVE(
 * 			cantor.x-cantor.measure*2/3,
 * 			cantor.y+cantor.measure*2/3,
 * 			cantor.z+cantor.measure*2/3,
 * 			cantor.measure/3
 * 			)
 * 		...
		 *append(PRIMITIVE(
 * 			cantor.x-cantor.measure*2/3,
 * 			cantor.y-cantor.measure*2/3,
 * 			cantor.z-cantor.measure*2/3,
 * 			cantor.measure/3
 * 			)

 */
#include <procgen/derivation/derivation.h>
#include <procgen/derivation/appender.h>
#include "obj.h"
#include <cassert>
void cantor_define(std::shared_ptr<Derivation> der, std::shared_ptr<TypeRegister> tr, std::shared_ptr<FunctionReg> fr)
{

	auto floatType = tr->getTypeId("float");
	auto floatVector = {floatType, floatType,floatType,floatType};

	tr->addComposite("cantor", floatVector);

	
	// Create always true rule
	
	auto cantorInput= tr->sharedResource("cantor");
	assert(cantorInput);

	auto boolReturn = tr->sharedResource("bool");
	assert(boolReturn);
	*(bool*) boolReturn->getData() = true;

	auto emptyBody = std::make_shared<Body>();
	// Register 'alwaysReturn' boolean predicate
	fr->addCompositeFunction("alwaysReturn", emptyBody,{cantorInput}, boolReturn);


	// Now proceeed to rule 1
	
	auto ruleInput = tr->sharedResource("cantor");
	auto primitiveOut = tr->sharedResource("primitive");

	auto constZero= tr->sharedResource("int");
	auto constOne = tr->sharedResource("int");
	auto constTwo= tr->sharedResource("int");
	auto constThree= tr->sharedResource("int");

	*(int*) constZero->getData() = 0;
	*(int*) constOne->getData() = 1;
	*(int*) constTwo->getData() = 2;
	*(int*) constThree->getData() = 3;

		
	auto cantorXget = std::make_shared<CompositeGet>();
	cantorXget->bindInput(0, fr->getHandler(ruleInput));
	cantorXget->bindInput(1, fr->getHandler(constZero));
	auto cantorYget = std::make_shared<CompositeGet>();
	cantorYget->bindInput(0, fr->getHandler(ruleInput));
	cantorYget->bindInput(1, fr->getHandler(constOne));
	auto cantorZget = std::make_shared<CompositeGet>();
	cantorZget->bindInput(0, fr->getHandler(ruleInput));
	cantorZget->bindInput(1, fr->getHandler(constTwo));
	auto cantorMeasureget = std::make_shared<CompositeGet>();
	cantorMeasureget->bindInput(0, fr->getHandler(ruleInput));
	cantorMeasureget->bindInput(1, fr->getHandler(constThree));


/*
 * 		append(CANTOR(cantor.x-(2/3)*cantor.width,
 * 			cantor.y+20,cantor.width/3)
 * 		append(CANTOR(cantor.x+(2/3)*cantor.width,
 * 			cantor.y+20,cantor.width/3)
 */

	auto outCantor = tr->sharedResource("cantor");


	auto mul = fr->getFunc("Mul:float");
	auto constTwoThirds = tr->sharedResource("float");
	auto mulResult= tr->sharedResource("float");
	*(float*) constTwoThirds->getData() = 2.f/3;
	mul->bindInput(0, cantorMeasureget);
	mul->bindInput(1, fr->getHandler(constTwoThirds));
	mul->bindOutput(mulResult);

	auto newXresult= tr->sharedResource("float");
	auto newX = fr->getFunc("Sub:float");
	newX->bindInput(0, cantorXget);
	newX->bindInput(1, mul);
	newX->bindOutput(newXresult);

	auto newX2result= tr->sharedResource("float");
	auto newX2 = fr->getFunc("Add:float");
	newX2->bindInput(0, cantorXget);
	newX2->bindInput(1, mul);
	newX2->bindOutput(newX2result);

	auto newYresult= tr->sharedResource("float");
	auto newY = fr->getFunc("Sub:float");
	newY->bindInput(0, cantorYget);
	newY->bindInput(1, mul);
	newY->bindOutput(newYresult);

	auto newY2result= tr->sharedResource("float");
	auto newY2 = fr->getFunc("Add:float");
	newY2->bindInput(0, cantorYget);
	newY2->bindInput(1, mul);
	newY2->bindOutput(newY2result);


	auto newZresult= tr->sharedResource("float");
	auto newZ = fr->getFunc("Sub:float");
	newZ->bindInput(0, cantorZget);
	newZ->bindInput(1, mul);
	newZ->bindOutput(newZresult);

	auto newZ2result= tr->sharedResource("float");
	auto newZ2 = fr->getFunc("Add:float");
	newZ2->bindInput(0, cantorZget);
	newZ2->bindInput(1, mul);
	newZ2->bindOutput(newZ2result);


	auto constFloatThree= tr->sharedResource("float");
	*(float*) constFloatThree->getData() = 3.0f;

	auto newMeasureresult= tr->sharedResource("float");
	auto newMeasure = fr->getFunc("Div:float");
	newMeasure->bindInput(0, cantorMeasureget);
	newMeasure->bindInput(1, fr->getHandler(constFloatThree));
	newMeasure->bindOutput(newMeasureresult);


	// set
	
	auto setCantorX = std::make_shared<CompositeSet>();
	setCantorX->bindInput(0, fr->getHandler(outCantor));
	setCantorX->bindInput(1, fr->getHandler(constZero));
	setCantorX->bindInput(2, fr->getHandler(newXresult));

	auto setCantorX2 = std::make_shared<CompositeSet>();
	setCantorX2->bindInput(0, fr->getHandler(outCantor));
	setCantorX2->bindInput(1, fr->getHandler(constZero));
	setCantorX2->bindInput(2, fr->getHandler(newX2result));

	auto setCantorY = std::make_shared<CompositeSet>();
	setCantorY->bindInput(0, fr->getHandler(outCantor));
	setCantorY->bindInput(1, fr->getHandler(constOne));
	setCantorY->bindInput(2, fr->getHandler(newYresult));

	auto setCantorY2 = std::make_shared<CompositeSet>();
	setCantorY2->bindInput(0, fr->getHandler(outCantor));
	setCantorY2->bindInput(1, fr->getHandler(constOne));
	setCantorY2->bindInput(2, fr->getHandler(newY2result));

	auto setCantorZ = std::make_shared<CompositeSet>();
	setCantorZ->bindInput(0, fr->getHandler(outCantor));
	setCantorZ->bindInput(1, fr->getHandler(constTwo));
	setCantorZ->bindInput(2, fr->getHandler(newZresult));

	auto setCantorZ2 = std::make_shared<CompositeSet>();
	setCantorZ2->bindInput(0, fr->getHandler(outCantor));
	setCantorZ2->bindInput(1, fr->getHandler(constTwo));
	setCantorZ2->bindInput(2, fr->getHandler(newZ2result));


	auto setCantorMeasure = std::make_shared<CompositeSet>();
	setCantorMeasure->bindInput(0, fr->getHandler(outCantor));
	setCantorMeasure->bindInput(1, fr->getHandler(constThree));
	setCantorMeasure->bindInput(2, fr->getHandler(newMeasureresult));



	auto appendCantor= std::make_shared<AppendSymbol>(der);
	appendCantor->bindInput(0, fr->getHandler(outCantor));

	auto ruleBody = std::make_shared<Body>();

	/* Calculate new data */
	ruleBody->appendStatement(newX);
	ruleBody->appendStatement(newX2);

	ruleBody->appendStatement(newY);
	ruleBody->appendStatement(newY2);

	ruleBody->appendStatement(newZ);
	ruleBody->appendStatement(newZ2);
	ruleBody->appendStatement(newMeasure);

	ruleBody->appendStatement(setCantorMeasure);
	/* Combine 8 times*/

#define CANTOR_CUBE(X,Y,Z)\
	ruleBody->appendStatement(setCantor##X);\
	ruleBody->appendStatement(setCantor##Y);\
	ruleBody->appendStatement(setCantor##Z);\
	ruleBody->appendStatement(appendCantor);

	CANTOR_CUBE(X,Y,Z);
	CANTOR_CUBE(X2,Y,Z);

	CANTOR_CUBE(X,Y2,Z);
	CANTOR_CUBE(X2,Y2,Z);

	CANTOR_CUBE(X,Y,Z2);
	CANTOR_CUBE(X2,Y,Z2);

	CANTOR_CUBE(X,Y2,Z2);
	CANTOR_CUBE(X2,Y2,Z2);

	fr->addCompositeFunction("cantor_rule", ruleBody,{ruleInput}, nullptr);


}


// Print out result
void inspectResultSymbols(TypeId primitive, const std::vector<std::shared_ptr<Resource>>& symbols)
{

	Object o;
	std::cout << ">>>>>>> RESULTING OBJECT >>>>>>\n";
	for(auto &x: symbols)
	{
		if(x->getBaseId() == primitive)
		{
			auto primitiveObject = std::dynamic_pointer_cast<CompositeResource>(x);
			float x= *(float*) primitiveObject->getComponent(0)->getData();
			float y= *(float*) primitiveObject->getComponent(1)->getData();
			float z= *(float*) primitiveObject->getComponent(2)->getData();
			float measure= *(float*) primitiveObject->getComponent(3)->getData();
			std::cout << "Primitive object: " << x << " " << y <<  " " << z << " " << measure <<std::endl;
			o.addPrimitive(glm::vec3(x,y,z),glm::vec3(measure,measure,measure));
		} else {
			auto cantorObject = std::dynamic_pointer_cast<CompositeResource>(x);
			float x= *(float*) cantorObject->getComponent(0)->getData();
			float y= *(float*) cantorObject->getComponent(1)->getData();
			float width= *(float*) cantorObject->getComponent(2)->getData();
			float heigth = *(float*) cantorObject->getComponent(3)->getData();
			std::cout << "Cantor object: " << x << " " << y <<  " " << width << " " << heigth<<std::endl;
			
		}
	}

	o.dumpVertices("3dcantor.obj");

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
	

	cantor_define(derivation,tr,fr);

	auto cantorCond= fr->getFunc("alwaysReturn");
	auto cantorProc= fr->getFunc("cantor_rule");


	derivation->addRule(tr->getTypeId("cantor"), cantorCond,cantorProc);

/*
 * Starting symbol
 * 	pyramide(50, 0)
 */

	auto startcantor = std::dynamic_pointer_cast<CompositeResource>(tr->sharedResource("cantor"));
	assert(startcantor);

	*(float*) startcantor->getComponent(0)->getData() = 0;
	*(float*) startcantor->getComponent(1)->getData() = 0;
	*(float*) startcantor->getComponent(2)->getData() = 0;
	*(float*) startcantor->getComponent(3)->getData() = 100;

	std::vector<std::shared_ptr<Resource>> symbols = {startcantor};

	derivation->setStartSymbols(symbols);

/*
 * Run generation
 */
	derivation->generate(iterationCount);
	
	auto syms= derivation->getCurrentSymbolList();

	inspectResultSymbols(tr->getTypeId("cantor"), syms);


	return 0;
}
