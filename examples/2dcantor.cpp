/*
 * 2017/12/16 Roman Dobias
 *
 * 2D cantor set fractal
 *
 * CANTOR
 * 	float x
 * 	float y
 * 	float width
 * 	float heigth 
 * PRIMITIVE:
 * 	float x
 * 	float y
 * 	float width
 * 	float heigth 
 *
 * RULES:
 * 	rule 1 (cantor)(always):
 * 		append(PRIMITIVE(cantor.x,cantor.y,cantor.width,cantor.heigth)
 * 		append(CANTOR(cantor.x-(2/3)*cantor.width,
 * 			cantor.y+20,cantor.width/3,cantor.heigth/3)
 * 		append(CANTOR(cantor.x+(2/3)*cantor.width,
 * 			cantor.y+20,cantor.width/3,cantor.heigth/3)
 *
 */
#include "derivation.h"
#include "appender.h"
#include "obj.h"
#include <cassert>
void cantor_define(std::shared_ptr<Derivation> der, std::shared_ptr<TypeRegister> tr, std::shared_ptr<FunctionReg> fr)
{

	auto floatType = tr->getTypeId("float");
	auto floatVector = {floatType, floatType,floatType,floatType};

	tr->addComposite("cantor", floatVector);
	tr->addComposite("primitive", floatVector);

	
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
	auto cantorWidthget = std::make_shared<CompositeGet>();
	cantorWidthget->bindInput(0, fr->getHandler(ruleInput));
	cantorWidthget->bindInput(1, fr->getHandler(constTwo));

	auto cantorHeigthget = std::make_shared<CompositeGet>();
	cantorHeigthget->bindInput(0, fr->getHandler(ruleInput));
	cantorHeigthget->bindInput(1, fr->getHandler(constThree));


/*
 *
 * 		append(PRIMITIVE(cantor.x,cantor.y,cantor.width)
 *
 */
	auto primitiveXset = std::make_shared<CompositeSet>();	
	primitiveXset->bindInput(0, fr->getHandler(primitiveOut));
	primitiveXset->bindInput(1, fr->getHandler(constZero));
	primitiveXset->bindInput(2, cantorXget);

	auto primitiveYset = std::make_shared<CompositeSet>();	
	primitiveYset->bindInput(0, fr->getHandler(primitiveOut));
	primitiveYset->bindInput(1, fr->getHandler(constOne));
	primitiveYset->bindInput(2, cantorYget);

	auto primitiveWidthset = std::make_shared<CompositeSet>();	
	primitiveWidthset->bindInput(0, fr->getHandler(primitiveOut));
	primitiveWidthset->bindInput(1, fr->getHandler(constTwo));
	primitiveWidthset->bindInput(2, cantorWidthget);

	auto primitiveHeigthset = std::make_shared<CompositeSet>();	
	primitiveHeigthset->bindInput(0, fr->getHandler(primitiveOut));
	primitiveHeigthset->bindInput(1, fr->getHandler(constThree));
	primitiveHeigthset->bindInput(2, cantorHeigthget);



	auto appendPrimitive= std::make_shared<AppendSymbol>(der);
	appendPrimitive->bindInput(0, fr->getHandler(primitiveOut));


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
	mul->bindInput(0, cantorWidthget);
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



	auto yOffset= tr->sharedResource("float");
	*(float*) yOffset->getData() = 1.0f;
	auto offsetYresult= tr->sharedResource("float");
	auto offsetY = fr->getFunc("Add:float");
	offsetY->bindInput(0, fr->getHandler(yOffset));
	offsetY->bindInput(1, cantorHeigthget);
	offsetY->bindOutput(offsetYresult);


	auto newYresult= tr->sharedResource("float");
	auto newY = fr->getFunc("Add:float");
	newY->bindInput(0, cantorYget);
	newY->bindInput(1, offsetY);
	newY->bindOutput(newYresult);

	auto constFloatThree= tr->sharedResource("float");
	*(float*) constFloatThree->getData() = 3.0f;

	auto newWidthresult= tr->sharedResource("float");
	auto newWidth = fr->getFunc("Div:float");
	newWidth->bindInput(0, cantorWidthget);
	newWidth->bindInput(1, fr->getHandler(constFloatThree));
	newWidth->bindOutput(newWidthresult);

	auto newHeigthresult= tr->sharedResource("float");
	auto newHeigth = fr->getFunc("Div:float");
	newHeigth->bindInput(0, cantorHeigthget);
	newHeigth->bindInput(1, fr->getHandler(constFloatThree));
	newHeigth->bindOutput(newHeigthresult);


	
	auto setCantorX = std::make_shared<CompositeSet>();
	setCantorX->bindInput(0, fr->getHandler(outCantor));
	setCantorX->bindInput(1, fr->getHandler(constZero));
	setCantorX->bindInput(2, fr->getHandler(newXresult));

	auto setCantorY = std::make_shared<CompositeSet>();
	setCantorY->bindInput(0, fr->getHandler(outCantor));
	setCantorY->bindInput(1, fr->getHandler(constOne));
	setCantorY->bindInput(2, fr->getHandler(newYresult));

	auto setCantorWidth = std::make_shared<CompositeSet>();
	setCantorWidth->bindInput(0, fr->getHandler(outCantor));
	setCantorWidth->bindInput(1, fr->getHandler(constTwo));
	setCantorWidth->bindInput(2, fr->getHandler(newWidthresult));

	auto setCantorHeigth = std::make_shared<CompositeSet>();
	setCantorHeigth->bindInput(0, fr->getHandler(outCantor));
	setCantorHeigth->bindInput(1, fr->getHandler(constThree));
	setCantorHeigth->bindInput(2, fr->getHandler(newHeigthresult));

	auto appendCantor= std::make_shared<AppendSymbol>(der);
	appendCantor->bindInput(0, fr->getHandler(outCantor));

	auto setCantorX2 = std::make_shared<CompositeSet>();
	setCantorX2->bindInput(0, fr->getHandler(outCantor));
	setCantorX2->bindInput(1, fr->getHandler(constZero));
	setCantorX2->bindInput(2, fr->getHandler(newX2result));


	auto ruleBody = std::make_shared<Body>();

	ruleBody->appendStatement(primitiveXset);
	ruleBody->appendStatement(primitiveYset);
	ruleBody->appendStatement(primitiveWidthset);
	ruleBody->appendStatement(primitiveHeigthset);
	ruleBody->appendStatement(appendPrimitive);

	ruleBody->appendStatement(newX);
	ruleBody->appendStatement(newX2);
	ruleBody->appendStatement(newY);
	ruleBody->appendStatement(newWidth);
	ruleBody->appendStatement(newHeigth);

	ruleBody->appendStatement(setCantorX);
	ruleBody->appendStatement(setCantorY);
	ruleBody->appendStatement(setCantorWidth);
	ruleBody->appendStatement(setCantorHeigth);
	ruleBody->appendStatement(appendCantor);

	ruleBody->appendStatement(setCantorX2);
	ruleBody->appendStatement(appendCantor);

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
			float width = *(float*) primitiveObject->getComponent(2)->getData();
			float heigth= *(float*) primitiveObject->getComponent(3)->getData();
			std::cout << "Primitive object: " << x << " " << y <<  " " << width << " " << heigth <<std::endl;
			o.addPrimitive(glm::vec3(x,y,0.0),glm::vec3(width,heigth*0.5,10.0f));
		} else {
			auto cantorObject = std::dynamic_pointer_cast<CompositeResource>(x);
			float x= *(float*) cantorObject->getComponent(0)->getData();
			float y= *(float*) cantorObject->getComponent(1)->getData();
			float width= *(float*) cantorObject->getComponent(2)->getData();
			float heigth = *(float*) cantorObject->getComponent(3)->getData();
			std::cout << "Cantor object: " << x << " " << y <<  " " << width << " " << heigth<<std::endl;
			
		}
	}

	o.dumpVertices("2dcantor.obj");

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
	*(float*) startcantor->getComponent(2)->getData() = 100;
	*(float*) startcantor->getComponent(3)->getData() = 100;

	auto start2cantor = std::dynamic_pointer_cast<CompositeResource>(tr->sharedResource("cantor"));
	assert(start2cantor);

	*(float*) start2cantor->getComponent(0)->getData() = 0;
	*(float*) start2cantor->getComponent(1)->getData() = -250;
	*(float*) start2cantor->getComponent(2)->getData() = 100;
	*(float*) start2cantor->getComponent(3)->getData() = -100;


	std::vector<std::shared_ptr<Resource>> symbols = {startcantor,start2cantor};

	derivation->setStartSymbols(symbols);

/*
 * Run generation
 */
	derivation->generate(iterationCount);
	
	auto syms= derivation->getCurrentSymbolList();

	inspectResultSymbols(tr->getTypeId("primitive"), syms);


	return 0;
}
