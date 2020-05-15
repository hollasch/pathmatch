cmake_minimum_required(VERSION 3.2)

set (CMAKE_CXX_STANDARD 17)

project (pathmatch LANGUAGES CXX)

add_executable (pathmatch
    src/pathmatch.cpp
    src/PathMatcher/pathmatcher.h
    src/PathMatcher/pathmatcher.cpp
    src/WildComp/wildcomp.h
    src/WildComp/wildcomp.cpp
)

include_directories(src src/PathMatcher src/WildComp)
