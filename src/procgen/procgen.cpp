#include <procgen/procgen.h>
#include <procgen/parser/generation.h>

namespace ProcGen {

	Procgen::Procgen()
	{
	}
	
    Procgen::~Procgen()
    {
    }

	bool Procgen::parseFile(const std::string& file)
	{
        std::ifstream s(file.c_str(),std::ifstream::in);
        generation._scanner->switch_streams(&s, &std::cerr);
        std::string* newString = new std::string(file);
        generation._location.initialize(newString);
        generation._parser->parse();
        
        s.close();

        if(!generation.hasAnyCompilationError())
            return true;
        return false;
	}

    bool Procgen::runInit()
    {
        // clear all
        this->generation.der->clear();
        auto initFunction = generation.functionregister->getFunc("init");
        if(initFunction == nullptr)
        {
            generation.errorMessage("Missing init() function");
            return false;
        }
        RunStatus rs;
        (*initFunction)(rs);
        //std::cout << "Initialized...\n";
        return true;
    }
    
    bool Procgen::run(int maximumSteps)
    {
        generation.der->generate(maximumSteps);
        return true;
    }

    json Procgen::serialize() const
    {
        return generation.der->to_json();
    }    

    void Procgen::setDebugOn(bool state)
    {
        //yydebug = 0;
        //if(state)
        //    yydebug = 1;
    }
}
