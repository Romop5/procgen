cmake_minimum_required(VERSION 2.6)
option(BUILD_TESTS "Build tests - use make test" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)

project(ibp)


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

add_library(procedural ${INTERPRET_SOURCE} ${DERIVATION_SOURCE})
target_compile_features(procedural PUBLIC cxx_auto_type)
target_include_directories(procedural PUBLIC src/interpret src/derivation)

###############################################################################
############    TESTS                                      ####################
###############################################################################

if (BUILD_TESTS)

    enable_testing()

    set(TEST_SOURCES 
        test/catch.cpp
        test/collection.cpp
        test/compositeComponents.cpp
        test/composite.cpp
        test/dynamicboxes.cpp
        test/functionreg.cpp
        test/return.cpp
        test/test.cpp
        test/typereg.cpp
        test/variablereg.cpp
    )


    add_executable(test_exe ${TEST_SOURCES})
    target_link_libraries(test_exe procedural)
    add_test(NAME unit_test COMMAND test_exe)

    add_executable(userfunc test/userfunc.cpp)
    target_link_libraries(userfunc procedural)


endif(BUILD_TESTS)
################################################################################
############    EXAMPLES                                    ####################
###############################################################################

if (BUILD_EXAMPLES)

    add_executable(algae examples/algae.cpp)
    target_link_libraries(algae procedural)

    add_executable(dom examples/dom.cpp)
    target_link_libraries(dom procedural)

    find_package(glm REQUIRED)
    add_executable(pyramide examples/pyramide.cpp)
    target_link_libraries(pyramide procedural)
    target_link_libraries(pyramide glm)

    add_executable(stochasticpyramide examples/stochasticpyramide.cpp)
    target_link_libraries(stochasticpyramide procedural)
    target_link_libraries(stochasticpyramide glm)

    add_executable(2dcantor examples/2dcantor.cpp)
    target_link_libraries(2dcantor procedural)
    target_link_libraries(2dcantor glm)

    add_executable(3dcantor examples/3dcantor.cpp)
    target_link_libraries(3dcantor procedural)
    target_link_libraries(3dcantor glm)

endif(BUILD_EXAMPLES)

