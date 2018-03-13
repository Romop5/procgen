# Tutorial

## Preface
This is a simple tutorial to understand how this library works. 
    Let our goal by the generation of [Sierpinski triangle](https://en.wikipedia.org/wiki/Sierpinski_triangle) in 3D space.

## Sierpinski triangle

Before moving to code writing, one must come up with a plan how recursive structures are built. Moreover, it's desired to build them using 
this library, so we will make use of recursion and rewritting process which are fundamental part of **L-systems** and this language as well.

Upon analysing the structure, a simple rule can be noted. There is a cube in the beginning and every cube consist of 4 cubes itself.
Thus, we need to somehow describe cube and the rule which inducively (using rewritting) makes up the whole pyramide.

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
    float size;
};

```

The structure above could be extended to handle cube's color, rotation or axis-dependent scaling. However, that is left for reader as an excercise.
    

