cmake_minimum_required(VERSION 2.6)
project(ibp)
file(GLOB SOURCES "src/*.cpp")

enable_testing()
add_test(NAME MyAwesomeTest COMMAND ibp-test)


add_executable(ibp-test src/test.cpp)
add_executable(ibp src/main.cpp)
