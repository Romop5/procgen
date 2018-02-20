#include <procgen/parser/procgen.h>

using namespace ProcGen;
int main(int argc, char** argv)
{

	if(argc < 3)
	{
		error(0,0,"USAGE: %s <FILE-PATH> <ITERATION-COUNT>",argv[0]);
		exit(1);
	}

	size_t iterationCount = atoi(argv[2]);

	ProcGen::Generation pg;
    pg.setDebugOn(false);

	pg.parseFile(argv[1]);
	if(pg.isReady())
	{
        if(pg.runInit() == false)
            return 1;
		std::cout << "It's dones" << std::endl;
		pg.run(iterationCount);
        json result = pg.serialize();
        std::cout << "Result" << result.dump(1) << "\n";
        
	} else {
        std::cout << "Failed...\n";
    }
}
