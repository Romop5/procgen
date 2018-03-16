# Complete ProcGen grammar
## Overview
This page describes ProcGen language from the non-technical, rather human-friendly view.
In case you are interested in exact parsing, have a look at [Complete grammar](COMPLETEGRAMMAR.md), which is a grammar in [ABNF](https://en.wikipedia.org/wiki/Augmented_Backus%E2%80%93Naur_form).

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

# Expressions

Expressions are trees which return value of some type. They are made of operators, variables, function calls or built-in special constructions.
## Operators
ProcGen provides both unary and binary operators. There is the precence table of operators below. The lower the operator is, the higher precedence it has:
```
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


# Definitions
## Type definition
## Rule definition
## Function definition


# Built-in
## Native functions


