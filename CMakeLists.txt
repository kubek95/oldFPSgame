cmake_minimum_required(VERSION 3.0)
project(oldFPSgame LANGUAGES CXX VERSION 0.1)

add_compile_options(-std=c++17)

add_library(compiler_warnings INTERFACE)

include(cmake/CompilerWarnings.cmake)
enable_compiler_warnings(compiler_warnings)

include(cmake/StaticAnalyzer.cmake)

add_subdirectory(src)
