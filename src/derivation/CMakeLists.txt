cmake_minimum_required(VERSION 2.6)
project(ibp)

include_directories(../interpret/)


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
	derivation.cpp
	algae.cpp
	)

add_executable(derivation
    ${INTERPRET_SRC} 
)

#add_executable(boxesTest test/test.cpp ${INTERPRET_SOURCE})

