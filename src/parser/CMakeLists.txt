cmake_minimum_required(VERSION 2.6)
project(ibp)

find_package(BISON)
find_package(FLEX)

include_directories(../interpret/)
BISON_TARGET(Parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cc COMPILE_FLAGS -d)
message(STATUS "FOO is ${BISON_Parser_OUTPUT_HEADER}")
FLEX_TARGET(Scanner lexer.l  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})


set(INTERPRET_SRC 
	../interpret/compositefunction.h
	../interpret/compositeutils.h
	../interpret/function.cpp
	../interpret/function.h
	../interpret/functionreg.cpp
	../interpret/functionreg.h
	../interpret/resource.cpp
	../interpret/resource.h
	../interpret/interpret.h
	../interpret/statement.cpp
	../interpret/statement.h
	../interpret/std.cpp
	../interpret/std.h
	../interpret/typedesc.cpp
	../interpret/typedesc.h
	../interpret/typereg.h
	../interpret/types.h
	../interpret/types.cpp
	../interpret/variablereg.cpp
	../interpret/variablereg.h
	)

add_executable(parser
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Scanner_OUTPUTS}
    ${INTERPRET_SRC} 
)

#add_executable(boxesTest test/test.cpp ${INTERPRET_SOURCE})

