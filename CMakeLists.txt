cmake_minimum_required(VERSION 2.6)
project(ProcGen)

option(PROCGEN_TESTS "Build tests - use make test" OFF)
option(PROCGEN_EXAMPLES "Build examples" OFF)
option(PROCGEN_PARSER "Build example parser" OFF)


# Add vendors (3rd libraries)

add_subdirectory(vendors)

###############################################################################
############    DEFINE SOURCE FILES 			   ####################
###############################################################################


set(SRC_PATH ${CMAKE_SOURCE_DIR}/src/procgen)

set(INTERPRET_SOURCE 
    src/procgen/interpret/function.cpp
    src/procgen/interpret/types.cpp
    src/procgen/interpret/functionreg.cpp
    src/procgen/interpret/typedesc.cpp
    src/procgen/interpret/std.cpp
    src/procgen/interpret/variablereg.cpp
    src/procgen/interpret/statement.cpp
    src/procgen/interpret/resource.cpp
)

set(INTERPRET_HEADERS
    src/procgen/interpret/functionreg.h
    src/procgen/interpret/compositefunction.h
    src/procgen/interpret/typedesc.h
    src/procgen/interpret/function.h
    src/procgen/interpret/std.h
    src/procgen/interpret/types.h
    src/procgen/interpret/typereg.h
    src/procgen/interpret/interpret.h
    src/procgen/interpret/resource.h
    src/procgen/interpret/statement.h
    src/procgen/interpret/compositeutils.h
    src/procgen/interpret/variablereg.h
)

set(DERIVATION_SOURCE
    src/procgen/derivation/derivation.cpp
)

set(DERIVATION_HEADERS
    src/procgen/derivation/natives.h
    src/procgen/derivation/appender.h
    src/procgen/derivation/derivation.h
    src/procgen/derivation/iteratorlimit.h
)

set(PARSER_HEADERS
    src/procgen/parser/utils.h
    src/procgen/parser/scanner.h
    src/procgen/parser/procgen.h
)

set(UTILS_HEADERS
    src/procgen/utils/logger.h
)



#add_library(procedural ${INTERPRET_SOURCE} ${DERIVATION_SOURCE})
#target_link_libraries(procedural json)
#target_compile_features(procedural PUBLIC cxx_auto_type)
#target_include_directories(procedural PUBLIC src/procgen/interpret src/procgen/derivation)
#target_include_directories(procedural PUBLIC $<INSTALL_INTERFACE:include>)
#target_include_directories(procedural PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Translate parser / lexer with Bison/Flex
BISON_TARGET(Parser src/procgen/parser/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cc COMPILE_FLAGS -d)
FLEX_TARGET(Scanner src/procgen/parser/lexer.l  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp COMPILE_FLAGS -d)
ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

add_library(procgen
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Scanner_OUTPUTS}
    ${INTERPRET_SOURCE}
    ${DERIVATION_SOURCE}
    src/procgen/parser/procgen.cpp
)


target_include_directories(procgen PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(procgen PUBLIC src/procgen/parser)
target_link_libraries(procgen json)


### Set up installation
install (FILES ${UTILS_HEADERS}
DESTINATION include/procgen/utils)

install (FILES ${INTERPRET_HEADERS}
DESTINATION include/procgen/interpret)

install (FILES ${DERIVATION_HEADERS}
DESTINATION include/procgen/derivation)

# Append bison and lexer headers
set(PARSER_HEADERS
    ${PARSER_HEADERS}
    ${BISON_Parser_OUTPUT_HEADER}
    ${CMAKE_CURRENT_BINARY_DIR}/stack.hh
    ${CMAKE_CURRENT_BINARY_DIR}/location.hh
    ${CMAKE_CURRENT_BINARY_DIR}/position.hh
)

install (FILES ${PARSER_HEADERS}
DESTINATION include/procgen/parser)

install (TARGETS procgen
DESTINATION lib)

###############################################################################
############    PARSER EXAMPLE 				   ####################
###############################################################################

if (PROCGEN_PARSER)

add_executable(parser test/parser.cpp)
target_link_libraries(parser procgen)

endif(PROCGEN_PARSER)



###############################################################################
############    TESTS                                      ####################
###############################################################################

if (PROCGEN_TESTS)

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


endif(PROCGEN_TESTS)
################################################################################
############    EXAMPLES                                    ####################
###############################################################################

if (PROCGEN_EXAMPLES)

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

endif(PROCGEN_EXAMPLES)

