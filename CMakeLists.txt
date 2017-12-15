cmake_minimum_required(VERSION 2.6)
project(ibp)

enable_testing()
add_test(NAME MyAwesomeTest COMMAND ibp-test)

add_compile_options("-std=c++11")

#message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
#message(STATUS "FOO is ${SOURCES}")
# Important, removes main.cpp from /src/ list
#list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
#message(STATUS "FOO is ${SOURCES}")
#add_executable(ibp src/main.cpp)


###############################################################################
############    DEFINE SOURCE FILES 			   ####################
###############################################################################

set(INTERPRET_SOURCE 
	src/interpret/compositefunction.h
	src/interpret/compositeutils.h
	src/interpret/function.cpp
	src/interpret/function.h
	src/interpret/functionreg.cpp
	src/interpret/functionreg.h
	src/interpret/resource.cpp
	src/interpret/resource.h
	src/interpret/statement.cpp
	src/interpret/statement.h
	src/interpret/std.cpp
	src/interpret/std.h
	src/interpret/typedesc.cpp
	src/interpret/typedesc.h
	src/interpret/typereg.h
	src/interpret/types.h
	src/interpret/types.cpp
	src/interpret/variablereg.cpp)

set(DERIVATION_SOURCE
	src/derivation/appender.h
	src/derivation/derivation.cpp
	src/derivation/derivation.h
)
###############################################################################
############    TESTS                                      ####################
###############################################################################

include_directories(src/interpret)

add_library(procedural ${INTERPRET_SOURCE} ${DERIVATION_SOURCE})

# Interpret tests
add_executable(boxesTest test/test.cpp)
target_link_libraries(boxesTest procedural)

add_executable(variabletest test/variablereg.cpp)
target_link_libraries(variabletest procedural)
add_executable(compositeType test/composite.cpp)
target_link_libraries(compositeType procedural)

add_executable(userfunc test/userfunc.cpp)
target_link_libraries(userfunc procedural)

add_executable(return test/return.cpp )
target_link_libraries(return procedural)
add_executable(collection test/collection.cpp )
target_link_libraries(collection procedural)
add_executable(composite test/composite.cpp )
target_link_libraries(composite procedural)
add_executable(compositeComponents test/compositeComponents.cpp )
target_link_libraries(compositeComponents procedural)

# Rewriting tests
include_directories(src/derivation)
add_executable(algae examples/algae.cpp)
target_link_libraries(algae procedural)

add_executable(dom examples/dom.cpp)
target_link_libraries(dom procedural)

find_package(glm REQUIRED)
add_executable(pyramide examples/pyramide.cpp)
target_link_libraries(pyramide procedural)
target_link_libraries(pyramide glm)
