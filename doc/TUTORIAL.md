Table of Contents
=================

   * [Tutorial](#tutorial)
      * [Preface](#preface)
      * [Sierpinski triangle](#sierpinski-triangle)
      * [Defining custom structures](#defining-custom-structures)
      * [Defining the rule](#defining-the-rule)
      * [Real rule defining](#real-rule-defining)
      * [How does it work ?](#how-does-it-work-)
      * [How can I visualise this](#how-can-i-visualise-this)
      * [Conclusion](#conclusion)
   * [Table of Contents](#table-of-contents)
# Tutorial

## Preface
This is a simple tutorial to understand how this library works. 
    Let our goal by the generation of [Sierpinski triangle](https://en.wikipedia.org/wiki/Sierpinski_triangle) in 3D space.

<p align="center">
    <img width="581" height="293" src="https://upload.wikimedia.org/wikipedia/commons/b/b4/Sierpinski_pyramid.png">
    <br>
    <b>Picture 0</b> Sierpinski pyramid. Photo by Wikipedia.
</p>



## Sierpinski triangle

Before moving to code writing, one must come up with a plan how recursive structures are built. Moreover, it's desired to build them using 
this library, so we will make use of recursion and rewritting process which are fundamental part of **L-systems** and this language as well.

Upon analysing the structure, a simple rule can be noted. There is a cube in the beginning and every cube consist of 4 cubes itself.
Thus, we need to somehow describe cube and the rule which inducively (using rewritting) makes up the whole pyramide.

<p align="center">
    <img width="581" height="193" src="https://github.com/Romop5/procgen/blob/master/doc/cubestep.png?raw=true">
    <br>
    <b>Picture 1</b> A simple pyramide can be achieved by iteratively transforming cubes into new cubes. Note that new cubes must be bounded by the bigger one.
</p>


**TODO**: add a nice SVG picture depicting CUBE transforming to new 4 CUBES

## Defining custom structures
Before we can actually generate anything, we need some structures that will carry pieces of information during the generation and finally, which will be interpreted as
the product of generation.

The language itself comes up with primitive types for decribing atomic values such as integral numbers (**int**), floating-point numbers (**float**) or truth value (**bool**).

In this procgen language, you can build up more comprehensive types out of primitives. For example, one could define a 3-part floating point vector
like this:

```cpp

using vec3 = struct 
{
    float x;
    float y;
    float z;
};
```

This structure allows use to store 3D positions and vector which is required to store cube's position.

Next, we need to describe cube itself. So, we define a cube structure to handle position and size of cube, like this:

```cpp

using cube = struct
{
    vec3 position;
    float sz;
};

```

The structure above could be extended to handle cube's color, rotation or axis-dependent scaling. However, that is left for reader as an excercise.

## Defining the rule

Alright, now we can express and describe a cube. Next, we come up with generating logic which will carry out recursion.

In **L-systems**, there are 3 main parts - an aplhabet of symbols, a set of rules and the starting string.

As for generation, we gonna do the same. We have some kind of structures that gonna be used in generating - cubes. We also want to 
describe a rule that will transform a single cube into new 4 cubes. Last but not least, we want to start the whole process and turn it off eventually.

As for defining rules, you can define multiple rules for a single symbols. Whenever a symbols of specified king appears in derivation string, one of specified rules is used
to transform the symbol. Each rule consists of two parts - **predicate** and **procedure**.

**Predicate** determines whether the rule is applicable on the given symbol. 

**Procedure** is a special function which is applied when rule is valid and used for transforming. 

If more rules are applicable, then one of them is choosed randomly.

## Real rule defining

Okay, so we want to transform a cube to 4 rules everytime we see it. In order to achieve that, we only need a single rule:

```cpp

// Utility function
vec3 createVec3(float x, float y, float z)
{
    vec3 tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return tmp;
}


// Utility function
cube createCube(vec3 position, float sz)
{
    cube tmp;
    tmp.position = position;
    tmp.sz = sz;
    return tmp;
}

using cubeRule = rule cube
{
    // always true -> predicate is always true and the rule is always used
    return true;
} 
{
   
   float newSize = this.sz/3.0;
    
    // Create A
    appendSymbol(createCube(createVec3(this.x+newSize, this.y+newSize, this.z - newSize), newSize));

    // Create B
    appendSymbol(createCube(createVec3(this.x+newSize, this.y-newSize, this.z - newSize), newSize));

    // Create C
    appendSymbol(createCube(createVec3(this.x-newSize, this.y, this.z - newSize), newSize));

    // Create D
    appendSymbol(createCube(createVec3(this.x, this.y, this.z + newSize), newSize));

}
```

## How does it work ? 

Consider having a cube at pos (0,0,0) with size 100 in the beggining. 

The first iteration will transform this cube into:
    cube (50,50,-50),50
    cube (50,-50,-50),50
    cube (-50,0,-50),50
    cube (0,0,50),50

In the next iteration, the exactly same transformation happens with the rest 4 cubes.

## How can I visualise this

In the end, we end up with a list of cubes in JSON. In order to see a result of this, we can iterate over the list of cubes and display each of them.

To see an example of generation, have a look at [Romop5/noob-Engine](https://github.com/Romop5/noob-engine) which incorporates ProcGen library.

## Conclusion

It's really easy and flexible to use procgen to anythin which can be generated using recursion and custom structures. No limits are appled for structure type
and you can personally choose whether you want to use polygons or cubes.




