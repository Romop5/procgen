# How to compile at Linux

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

``
