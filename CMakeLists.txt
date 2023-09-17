cmake_minimum_required(VERSION 3.24)

project(BuildAndDispatch CXX)

set(SOURCES
    Source/Core/Main.cpp
)

add_executable(BuildAndDispatch ${SOURCES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source FILES ${SOURCES})

