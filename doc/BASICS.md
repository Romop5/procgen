# Basics of procgen language

This document covers the basics of procgen language. The procgen languge is an imperative, specified language based on **L-systems**.

*Imperative* means it uses commands to define the flow - you can literally define the sequence of code. The language is based on *L-systems* which means
you are basically working with symbols (implemented as structures) which are replaced with different symbols in each iteration according to rules. 

You can find out more about L-systems in [How does this tool work](HOWDOESITWORK.md)

## Library overview 

The procedural generation in this library boils down to two important concepts - **symbols** and **rewritting rules**. 

Generally, **symbol** is a typed value. Each symbol has a type and despite types haveing the same structure, they are distiquished.
Symbols can be simple atomic values like *integers*, *floats* or hierarchies built up upon these types. 

For each symbol, a set of rules can be defined. These rules are used when the symbol is about to *rewrite*. The output of rule might be a string of symbols, or 
just nothing. 

When no rules are defined for symbol and yet symbols appears in system, then *implicit rule* is used which basically makes a clone of rewritten symbol.


In order to start this process, there must be the starting string of symbols. This can be defined in special function *init* which is called on script init.

## Example of generating system

You can find an example in [How does this tool work](HOWDOESITWORK.md).

A real-world generating example is part of [Tutorial](TUTORIAL.md).


# Syntax

## Symbols

To specify a new symbol, one must use a struct-defining construction in form:

```cpp
using NEW_TYPE = struct {
    ITEM_TYPE_1 itemName,
    ITEM_TYPE_2 itemName,
    ...
    ITEM_TYPE_N itemName,
};
```




## Rules

## Functions

## Simple script


# Advanced topics

## Stopping the generation
## Context-accessing

