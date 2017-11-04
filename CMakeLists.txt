cmake_minimum_required(VERSION 2.6)
project(ibp)
file(GLOB SOURCES "src/*.cpp")

enable_testing()
include_directories(AFTER "src")
add_test(NAME MyAwesomeTest COMMAND ibp-test)

add_compile_options("-std=c++11")

#message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
#message(STATUS "FOO is ${SOURCES}")
# Important, removes main.cpp from /src/ list
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
#message(STATUS "FOO is ${SOURCES}")
add_executable(boxesTest test/test.cpp ${SOURCES})
#add_executable(typesTest test/typereg.cpp ${SOURCES})
add_executable(variabletest test/variablereg.cpp ${SOURCES})
add_executable(compositeType test/composite.cpp ${SOURCES})
add_executable(userfunc test/userfunc.cpp ${SOURCES})
add_executable(return test/return.cpp ${SOURCES})
#add_executable(ibp src/main.cpp)
