cmake_minimum_required(VERSION 3.24)

add_subdirectory(ThirdParty/glad)
add_subdirectory(ThirdParty/glfw)
add_subdirectory(ThirdParty/glm)

project(BuildAndDispatch CXX)

set(SOURCES
    Source/Core/Assert.h
    Source/Core/Logger.cpp
    Source/Core/Logger.h
    Source/Core/Main.cpp
    Source/Platform/File.h
    Source/Renderer/Buffer.cpp
    Source/Renderer/Buffer.h
    Source/Renderer/Renderer.cpp
    Source/Renderer/Renderer.h
    Source/Renderer/Shader.cpp
    Source/Renderer/Shader.h
    Source/Renderer/Window.cpp
    Source/Renderer/Window.h
    Source/Simulation/World.cpp
    Source/Simulation/World.h
    Source/Windows/File.cpp
    Source/Windows/File.h
)

set(LIBRARIES
    glad
    glfw
    glm
)

set(TARGET_NAME BuildAndDispatch)

add_executable(${TARGET_NAME} ${SOURCES})

target_compile_definitions(${TARGET_NAME} PRIVATE WIN32_LEAN_AND_MEAN WIN32_NO_MIN_MAX)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_link_libraries(${TARGET_NAME} PRIVATE ${LIBRARIES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 23 CXX_EXTENSIONS OFF)
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source FILES ${SOURCES})

