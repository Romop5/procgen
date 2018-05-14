/**
 * @file 'interpret/compositefunction.h'
 * @brief Declaration of function call and composite function mechanism
 * @copyright The MIT license 
 */
#ifndef COMPOSITE_H
#define COMPOSITE_H
#include <memory>
#include <procgen/interpret/function.h>
#include <procgen/interpret/resource.h>
#include <procgen/interpret/statement.h>
#include <procgen/utils/logger.h>
#include <string>
#include <vector>

/*
1. Create an expression tree with vars
2. Define a subset of public variables and output
3. Register tree, public vars and output

4. Create function call box for this composite function.
	On call, call box will set new content of public variables, process the called function
	and copy the result value back.

5. Enjoy it
*/

namespace ProcGen {
// Will handle tree and interfaces
struct CompositeFunction {
    std::vector<std::shared_ptr<Resource>> inputs;
    std::shared_ptr<Resource> output;
    std::shared_ptr<Statement> core;

public:
    CompositeFunction(
        std::shared_ptr<Statement> core,
        std::vector<std::shared_ptr<Resource>> inputs,
        std::shared_ptr<Resource> output);
};

class FunctionCall : public Function {
    std::weak_ptr<CompositeFunction> cf;
    std::string name;

public:
    FunctionCall(std::shared_ptr<CompositeFunction> cf, std::string name);

    bool operator()(RunStatus& stat);
    bool bindInput(size_t id, std::shared_ptr<Function> func) override;
    bool bindOutput(std::shared_ptr<Resource> res) override;
};
}
#endif
