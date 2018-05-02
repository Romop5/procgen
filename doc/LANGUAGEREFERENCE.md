# ProcGen Language reference
## Overview
This page describes ProcGen language from the non-technical, rather human-friendly view.
In case you are interested in exact parsing, have a look at [Complete grammar](COMPLETEGRAMMAR.md), which is a grammar in [ABNF](https://en.wikipedia.org/wiki/Augmented_Backus%E2%80%93Naur_form).

*ProcGen* language is based on C-like syntax, so if you have already encountered with any *C*-like language (C/C++, Java), you will find it easy to understand *ProcGen* syntax.
### Simple program
At the moment, you may wonder how does the language look like. So, let's have a simple example.

```cpp

// Parameter definition
parameter int iterations = 1;

// Struct definition
using point = struct {
    float a; float b; 
};

// Rule definition
using randomTransform = rule point
{
    return true;
} {
    // Variable declaration 
    float newA;
    float newB;
    // assignment with built-in function random()
    newA = this.a + random();
    newB = this.b + random();
    // automatic composite variable construction
    // and built-in function call for appending structure to string
    appendSymbol(point(newA,newB));
}
int init()
{
    appendSymbol(point(0.0,0.0));
    setMaximumIterations(3);
}
```
The script above will at first produce a single symbol *point* in *init()* function. Then, this symbols is going to be rewritten using *randomTransform* rule as the rule is available for this symbol. The rewritting process has *3* iterations. The result is a new *point* structure, which has randomized position.

## Reserved words
Just as the almost every programming language, even ProcGen uses English-based words with special meaning which are therefore reserved and cannot be used
for identifiers. 

Here is the total list of keywords:

```
using       struct      rule        parameter   if
else        while       return      typeid
convert     insert      at          size        any
del         int         float       bool        char
```

# Atomic types
In ProcGen, types are either atomic or are composed from these atomic types. Atomic types are *float*, *int* and *bool*.
# Expressions

Expressions are trees which return value of some type. They are made of operators, variables, function calls or built-in special constructions.
## Operators
ProcGen provides both unary and binary operators. There is the precence table of operators below. The lower the operator is, the higher precedence it has:
```cpp
&& ||
== != 
> <
+ - 
/ 
* 
- (unary)
+ (unary)
! (unary)

```

# Built-in expressions

Sometimes special functionality is desired couldn't be possible to achieve using pure language. Then it comes to introduce built-in expressions which serve the purpose.

## Typeid
In special cases when we manipule with containers, we need to determine what type is member of containter of in order to process the value.

Typeid introduces special notation which allows us to determine and compare types of variables or built-in types.

For example,
```cpp
typeid<int> == typeid<foo>
```
will return *true* in case variable *foo* has *int* type.
## Convert

As the language support special **any** type, which serves as container for any type, one must to determine what kind of value any type carries.
To ensure that **any** variable is of specified type, run-time type checking is involved.

Let *foo* be a variable of **any** type. Then using following construction we make sure that **foo** has int value.
```cpp
convert<int>(foo)
```
This effectively returns int value of *foo* or terminates program in case *foo* is not of int type.


# Statements
Statements stand for atomic steps which are taken when executing the program. Statements enable us to define logic which will gonna be triggered when program reach specified
location in source code.

We can classify statements into:
- **atomic statements** - do one thing, e.g. assignment to variable, variable definition
- **composite statements** - are made of several other statements. E.g. cycles, conditions 

## Declaration 
Declarations enable us to introduce new variable in rule or function. Variables are scoped - they are only valid till the end of block they are encapsuled with.

A typical declaration looks like:

```cpp
int myFancyNewVariable = 10;
```
Initialization of newly introduced varable is optional and can incorporate expressions.

## Assignement
Assignement are used to copy value of one variable to another one. In order to be valid, assignements must use the same variable types. In case of using **any** variable, 
**convert** must be used to specify terminate type of variable.

```cpp
oneVariable = anotherVariable;
```

## Condition
In order to branch the execution of code, conditions are used. Using brackets in ProcGen is *mandatory*. There are only two construtions - *if* and *else*, where *else* is optional.

```cpp
if( valid-bool-expression) 
{
    print(1);
} else {
    print(0);
}

```
## While-cycle
In order to repeat sequence of commands, generic *while-cycle* can be used.

```cpp
while ( true-bool-expression)
{
    doSomething();
}
```

# Definitions
## Type definition
To define a new type, struct is defined using already-defined types.

For example, to define a new type *player*, which will handle player's heigth, age and maturity flag, following code is used:
```cpp
using player = struct {
    float health;
    int age;
    bool isMature;
};
```

## Rule definition
Finally, rule definition is explained. Rules are fundamental part of ProcGen language - they describe how symbols are rewritten.
Each rule is defined for some symbol. Rule is made of *predicate* which determines if rule is applicable to given symbol, and *procedure* which defines
how symbol is rewritten.

```cpp
using simpleStructure = struct
{
    int i;
};
using exampleRule = rule simpleStructure
{
    // this rule is always applicable
    return true;
} {
    // take current value of i and increase it by one
    appendSymbol(simpleStructure(this.i + 1));
};
```
As you could see, current symbol which is being rewritten, is referenced as *this*.
## Function definition
Once a time it's neccessary to extract sequences of code, which could be reused somewhere else. In these cases, functions are introduced.

In ProcGen, function takes *parameters*, do some code and *returns value*. Alterning global values or using other functions is allowed here.

A simple function to find maximum of two numbers can be defined like this:

```cpp
float max(float a, float b)
{
    if(a > b)
    {
        return a;
    }
    return b;
}
```

## Native functions
Using constructions above, it's possible to write down a program which will define some structures and they way how they are rewritten. However, in order to access and control derivation mechanism, one must use specialized built-in functions for this purpose. 

### appendSymbol(symbol) 
Takes *symbol* and appends it into the end of current symbol string.
### int getCurrentPosition() 
Returns the position of symbol, which is currently being rewritten. This function makes sense while defining rule's predicate and procedure and can be used in utility functions.
### int getCurrentStringId() 
Likewise *getCurrentStringId*, this function returns the ID of current symbol string. Symbol string ID and position are required for functions like *getParent* or *getSymbol*. 
### bool hasSymbol(stringid, position)
Determines if there exists a symbol at position [stringId, position]. If not, false is returned.
### getSymbol(stringid, position) 
Returns the symbol at position [stringId, position]. It's reasonable to use *hasSymbol* before using *getSymbol*. This function returns symbol of *any* type and *convert* must be used to work with this type.
### int getParent(stringid, position)
Returns position of parent of symbol at [stringid, position]. Position for non-existing symbol is undefined.
### setMaximumIterations(int)
Sets the maximum iterations for rewriting system. 
### skipSymbol(int)
Skips rewriting of flowing N symbols. This allows to aggregate several symbols.


