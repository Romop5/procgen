//#include <procgen/parser/procgen.h>
#include <procgen/procgen.h>

using namespace ProcGen;
int main(int argc, char** argv)
{

	if(argc < 3)
	{
        fprintf(stderr, "USAGE: %s <FILE-PATH> <ITERATION-COUNT>",argv[0]);
		exit(1);
	}

	size_t iterationCount = atoi(argv[2]);

	ProcGen::Procgen pg;
    pg.setDebugOn(false);

	pg.parseFile(argv[1]);
	if(pg.isReady())
	{
        // set uniform
        pg.setUniform("ticks", 1000);
        auto initResult = pg.runInit();
        if(initResult == false)
        {
            if(pg.hasAnyErrorMessage())
                std::cout << pg.getLastErrorMessage() << std::endl;
            return 1;
        }
		std::cout << "It's dones" << std::endl;
		auto runResult = pg.run(iterationCount);
        if(runResult == false)
        {
            std::cout << "Error: " <<  pg.getLastErrorMessage() << std::endl;
        }

        json result = pg.serialize();
        std::cout << "Result" << result.dump(1) << "\n";
        
	} else {
        std::cout << "Failed...\n";
    }
}
