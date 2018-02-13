ProcGen - a library for procedural generation
================================

## About

*ProcGen* is a C++ library aimed to be used for procedural generation. It works as a blackbox that receives a description of generation, written down in **procedural language** at the input and outputs generated **collection of structures** in fancy *JSON* format.

The generation is similar to parametric L-systems as it has **rules** with **parameters**, which are **conditioned**. However, both rule body and condition are defined in **imperative manner**.

### Features
* C-like style of code
* Run-time configuration 
* JSON outputs
* CMake supported
* Python bindings (TODO)

[How does this tool work](HOWDOESITWORK.md)

### Example procedural code

```cpp
// create a basic structure
using helloStructure = struct
{
	int counter;
};

// rule for helloStructure
using simpleRule = rule helloStructure 
{
	// the rule is only valid when counter is not 5
	if(this.counter != 5)
		return true;
	return false;
}
{
	// rule's procedure
	helloStructure tmp = this;
	tmp.counter = tmp.counter + 1;
	// in this case, append structure with incremented counter for next-step of derivation
	appendSymbol(tmp);
};

// This is something like the main function
int init()
{
	// it appends symbol to start string
	helloStructure hello;
	hello.counter = 0;
	appendSymbol(hello);
}
```
## Example results
See https://imgur.com/a/xdzYu

## Dependencies
* bison
* flex
* nlohmann/json  
* catchorg/catch2

## Compilation 
Before trying to compile the project, make sure that you have bison and flex installed:

> apt-get install flex bison

The aim of this project is to be a CMake-complaint library. 

In order to compile the core of library, create a new directory `build` and run following command inside:
> cmake ../

Then use standard *make* pipeline.

> make

### Examples & Tests

Additionaly, you can compile examples and/or tests by specifing their presence *explicitly* in CMake command:

> cmake ../ -DBUILD_EXAMPLES  

or both:
> cmake ../ -DBUILD_EXAMPLES -DBUILD_TESTS

If tests are compiled, following command can be used to run them all:

> make test


## Special thanks to
* Ing. Tomáš Milet - advisory/supervision

