cmake_minimum_required(VERSION 3.2...3.27.9)

set (CMAKE_CXX_STANDARD 20)

project (pathmatch LANGUAGES CXX)

add_executable (pathmatch
    src/pathmatch.cpp
    src/PathMatcher/pathmatcher.h
    src/PathMatcher/pathmatcher.cpp
    src/WildComp/wildcomp.h
    src/WildComp/wildcomp.cpp
)

add_executable (pathmatcherTest
    src/PathMatcher/pathmatcher.h
    src/PathMatcher/pathmatcher.cpp
    src/WildComp/wildcomp.h
    src/WildComp/wildcomp.cpp
    src/PathMatcher/pathmatcherTest.cpp
)

include_directories(src src/PathMatcher src/WildComp)
