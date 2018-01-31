#include "procgen.h"

using namespace ProcGen;
int main(int argc, char** argv)
{

	if(argc < 2)
	{
		error(0,0,"USAGE: %s <FILE-PATH>",argv[0]);
		exit(1);
	}

	ProcGen::Generation pg;

	pg.parseFile(argv[1]);
	if(pg.isReady())
	{
		std::cout << "It's dones" << std::endl;
		//pg.run();
	}
}
