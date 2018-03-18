# Compiling at Windows

## Requirements

- mingw-32 (should work with Visual Studio, too)
- Git for Windows
- CMake for Windows
- win-flex-bison3 (3 is required as it supports C++ parser)

## Steps

1. Install all requirements
2. Append path to mingw to PATH (Enviroment settings in Windows)
3. Rename win-bison and win-flex to bison and flex, respectively.
This step is required in order to let CMake find flex / bison.

4. Run git clone / cmake as usual.
5. Use mingw32-make to make the project
6. Done

## FAQ

### FlexLexer.h missing
This file should be a part of win-flex-bison archive. Copy .h file to build directory and that's it.

### flex / bison missing (CMake error)

See step 3.

