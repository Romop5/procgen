ProcGen - a library for procedural generation
================================
<p align="center">
    <img width="120" height="90" src="https://github.com/Romop5/procgen/blob/master/pyramide.gif?raw=true">
</p>

## About
**WORK IN PROGRESS** *BUG REPORTS APPRECIATED*

*ProcGen* is a C++ library aimed to be used for procedural generation. It works as a blackbox that receives a description of generation, written down in **procedural language** at the input and outputs generated **collection of structures** in fancy *JSON* format.

The generation is similar to parametric L-systems as it has **rules** with **parameters**, which are **conditioned**. However, both rule body and condition are defined in **imperative manner**.

The library is a part of *bachelor thesis* at [FIT BUT](www.fit.vutbr.cz) in 2018.

### Features
* C-like style of code
* Run-time configuration 
* JSON outputs
* CMake supported
* Python bindings (TODO)

[How does this tool work](HOWDOESITWORK.md)

[Annotated BNF grammar]() TODO
[Tutorials]() TODO


### Example procedural code
Following code was used to generate the pyramide which can be found in the header of
this document.

```cpp
using vec3 = struct  {// define vector of 3 floats
    float x;
    float y;
    float z;
};

vec3 cVec3(float x, float y, float z) {// define 'constructor'
    vec3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}

using cube = struct {// define cube (position + side of cube)
    vec3 position;
    float sz;
};

cube cCube(vec3 pos, float sz) {// constructor
    cube tmp;
    tmp.position = pos;
    tmp.sz = sz;
    return tmp;
}

// the one and only rule
using tst = rule cube {
    return true;  // this rule is valid in every step, so simply return true
} {
    // the body of rule -> following statements create new symbols which
    // replace the current one in next-step string
    float newSize = this.sz/2.0;
    // Generate three base cubes
    appendSymbol(cCube(cVec3(this.position.x-newSize,this.position.y-newSize,this.position.z-newSize) ,newSize));
    appendSymbol(cCube(cVec3(this.position.x+newSize,this.position.y-newSize,this.position.z-newSize) ,newSize));
    appendSymbol(cCube(cVec3(this.position.x,this.position.y-newSize,this.position.z+newSize) ,newSize));
    // Generate one above them
    appendSymbol(cCube(cVec3(this.position.x,this.position.y+newSize,this.position.z) ,newSize));

};

// Global function which works as main() in regular c
int init() {
    appendSymbol(cCube(cVec3(0.0,0.0,0.0),50.0));
    setMaximumIterations(5);
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

