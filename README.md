ProcGen - a library for procedural generation
================================
<p align="center">
    <img width="120" height="90" src="https://github.com/Romop5/procgen/blob/master/pyramide.gif?raw=true">
</p>

## About
**WORK IN PROGRESS** *BUG REPORTS APPRECIATED*

Have you ever wanted to generate fractals ? Recursive geometry ? Complex self-similar structures ? Or simple 3D models for voxel-like game ?

*ProcGen* is a C++ library aimed to be used for procedural generation. It works as rewritting machine which **iteratively transforms user-defined structures to other structures**. This is allowed by defining custom structures like in C-like language and defining rules which act like procedures, transforming the given input structure to new structures.

The generation is thus similar to parametric stochastic OL-systems as it has **rules** with **parameters**, which are **conditioned**. However, both rule body and condition are defined in **imperative manner**, with C-like statements like **if**, **while**.

It works as a blackbox that receives a description of generation, written down in **procedural language** at the input and outputs generated **collection of structures** in fancy *JSON* format.


The library is a part of *bachelor thesis* at [FIT BUT](www.fit.vutbr.cz) in 2018.
## Example results
<p align="center">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/bullet.svg.png?raw=true" width="120px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/densetree.svg.png?raw=true" width="120px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/ker-10angle.svg.png?raw=true" width="120px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/newhash.svg.png?raw=true" width="120px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/nicehash.svg.png?raw=true" width="120px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/strom-30-06.svg.png?raw=true" width="120px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/strom-sixedge.svg.png?raw=true" width="120px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/turtle.snowflake.png?raw=true" width="120px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/turtle.labyrint.procgen.png?raw=true" width="120px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/turtle.classic.procgen.png?raw=true" width="120px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/turtle.tree.procgen.png?raw=true" width="120px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/turtle.tree2.procgen.png?raw=true" width="120px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/turtle.tree3.procgen.png?raw=true" width="120px">
</p>
<p align="center">
<img src="https://github.com/Romop5/noob-engine/blob/master/build/exampleProceduralPlane.png?raw=true" width="120px">
<img src="https://github.com/Romop5/noob-engine/blob/master/build/examplePyramide.png?raw=true" width="120px">
<img src="https://github.com/Romop5/noob-engine/blob/master/build/exampleRealTree.png?raw=true" width="120px">
<img src="https://github.com/Romop5/noob-engine/blob/master/build/exampleSiepiersky.png?raw=true" width="120px">
<img src="https://github.com/Romop5/noob-engine/blob/master/build/exampleTower.png?raw=true" width="120px">
<img src="https://github.com/Romop5/noob-engine/blob/master/build/exampleTree.png?raw=true" width="120px">

</p>

See 
* [Romop5/procgen-svg](https://github.com/Romop5/procgen-svg)
* [Romop5/noob-engine](https://github.com/Romop5/noob-engine)


### Features
* C-like style of code
* L-system & rewritting system based generation
* Run-time structure and rules configuration 
* JSON serializing for results of generation
* CMake supported
* Python bindings (TODO)

### Documentation
[Changelog](CHANGELOG.md)

[Simple generating tutorial](doc/TUTORIAL.md)

[How does this tool and L-systems work](HOWDOESITWORK.md)

[Complete BNF grammar](doc/COMPLETEGRAMMAR.md) 


### Example procedural code
Following code was used to generate the pyramide which can be found in the header of
this document.

```cpp
using vec3 = struct  {// define vector of 3 floats
    float x;
    float y;
    float z;
};

using cube = struct {// define cube (position + side of cube)
    vec3 position;
    float sz;
};

// the one and only rule
using tst = rule cube {
    return true;  // this rule is valid in every step, so simply return true
} {
    // the body of rule -> following statements create new symbols which
    // replace the current one in next-step string
    float newSize = this.sz/2.0;
    // Generate three base cubes
    appendSymbol(cube(vec3(this.position.x-newSize,this.position.y-newSize,this.position.z-newSize) ,newSize));
    appendSymbol(cube(vec3(this.position.x+newSize,this.position.y-newSize,this.position.z-newSize) ,newSize));
    appendSymbol(cube(vec3(this.position.x,this.position.y-newSize,this.position.z+newSize) ,newSize));
    // Generate one above them
    appendSymbol(cube(vec3(this.position.x,this.position.y+newSize,this.position.z) ,newSize));

};

// Global function which works as main() in regular c
int init() {
    appendSymbol(cCube(cVec3(0.0,0.0,0.0),50.0));
    setMaximumIterations(5);
}
```
## Dependencies
* [bison](https://www.gnu.org/software/bison/)
* [westes/flex](https://github.com/westes/flex)
* [nlohmann/json](https://github.com/nlohmann/json)
* [catchorg/catch2](https://github.com/catchorg/Catch2)

## Compilation

* [Compile under Linux](COMPILE-LINUX.md)
* [Compile under Windows](COMPILE-WINDOWS.md)


## Special thanks to
* Ing. Tomáš Milet - advisory/supervision

