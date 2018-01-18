# How does it work 

## L-systems

L-systems are special case of rewriting system where rewriting can be performed in pararell manner during a single step.

You can imagine L-systems as a group of *symbols*, which are replaced with a string symbols (possibly empty).

The best example of this system is the growth of green algae.
Consider following system:
~~~
Symbols:    A, B

Rules:
    A -> B
    B -> A B
~~~

For an initial string `A` this system performs following steps:

~~~
0. step      A
             |
1. step      B
            | |
2. step    A   B
          |   | |
3. step   B   A   B
         | |  |  | |
4. step  A B  B  A B

~~~
## Parametric L-systems
Now consider that symbols would have some parameters and these could specify if rule is applicaple or not, thus whether symbol should or shouldn't be rewritten.

Rules in parametric L-systems are specified with parameters and conditions.

## The procedural language

In this project, a new language is defined which has the power of parametric L-systems.

The language defines rules as procedures and conditions as functions.


