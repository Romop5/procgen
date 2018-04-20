#include <procgen/parser/generation.h>
#include <procgen/procgen.h>

namespace ProcGen {

Procgen::Procgen()
{
    // This is neccessary in order to produce message on catch
    this->lastError.reserve(255);
}

Procgen::~Procgen()
{
}

bool Procgen::parseFile(const std::string& file)
{
    std::ifstream s(file.c_str(), std::ifstream::in);
    generation._scanner->switch_streams(&s, &std::cerr);
    //std::string* newString = new std::string(file);

    std::shared_ptr<std::string> fileName = std::make_shared<std::string>(file);
    generation._location.initialize(fileName.get());
    generation._parser->parse();

    s.close();

    if (!generation.hasAnyCompilationError())
        return true;
    return false;
}

bool Procgen::runInit()
{
    try {
        generation.der->generate(0);
        // clear all
        this->generation.der->clear();
        auto initFunction = generation.functionregister->getFunc("init");
        if (initFunction == nullptr) {
            generation.errorMessage("Missing init() function");
            return false;
        }
        RunStatus rs;
        (*initFunction)(rs);
    } catch (const std::bad_alloc& ba) {
        this->lastError = "Memory exhaustion during generation";
        return false;
    } catch (const std::exception& exp) {
        this->lastError = exp.what();
        return false;
    } catch (...) {
        this->lastError = "Unknown error happened";
        return false;
    }

    return true;
}

bool Procgen::run(int maximumSteps)
{
    try {
        generation.der->generate(maximumSteps);
    } catch (const std::bad_alloc& ba) {
        this->lastError = "Memory exhaustion during generation";
        return false;
    } catch (const std::exception& exp) {
        this->lastError = exp.what();
        return false;
    } catch (...) {
        this->lastError = "Unknown error happened";
        return false;
    }
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

const std::string& Procgen::getLastErrorMessage() const
{
    return this->lastError;
}

bool Procgen::hasAnyErrorMessage() const
{
    return (!this->lastError.empty());
}
}
