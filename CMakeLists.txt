cmake_minimum_required(VERSION 3.24)

add_subdirectory(ThirdParty/glad)
add_subdirectory(ThirdParty/glfw)
add_subdirectory(ThirdParty/glm)
add_subdirectory(ThirdParty/json)

project(BuildAndDispatch CXX)

set(SOURCES
    Source/Core/Assert.h
    Source/Core/GameLoop.cpp
    Source/Core/GameLoop.h
    Source/Core/InputState.h
    Source/Core/Logger.cpp
    Source/Core/Logger.h
    Source/Core/Main.cpp
    Source/Core/Rect2D.h
    Source/Core/Transform.h
    Source/Layer/TrackLayer.cpp
    Source/Layer/TrackLayer.h
    Source/Layer/Layer.h
    Source/Platform/File.h
    Source/Platform/Time.h
    Source/Renderer/Buffer.cpp
    Source/Renderer/Buffer.h
    Source/Renderer/GeometryBuffer.h
    Source/Renderer/Renderer.cpp
    Source/Renderer/Renderer.h
    Source/Renderer/Shader.cpp
    Source/Renderer/Shader.h
    Source/Renderer/Texture.cpp
    Source/Renderer/Texture.h
    Source/Renderer/VectorIcon.cpp
    Source/Renderer/VectorIcon.h
    Source/Renderer/Window.cpp
    Source/Renderer/Window.h
    Source/Simulation/Route.h
    Source/Simulation/Signal.h
    Source/Simulation/Track.h
    Source/Simulation/Train.h
    Source/Simulation/World.cpp
    Source/Simulation/World.h
    Source/UI/Container.h
    Source/UI/Containers/CanvasContainer.cpp
    Source/UI/Containers/CanvasContainer.h
    Source/UI/Containers/StackContainer.cpp
    Source/UI/Containers/StackContainer.h
    Source/UI/Font.cpp
    Source/UI/Font.h
    Source/UI/RenderBuffer.cpp
    Source/UI/RenderBuffer.h
    Source/UI/Size.h
    Source/UI/TextLayout.cpp
    Source/UI/TextLayout.h
    Source/UI/Widget.cpp
    Source/UI/Widget.h
    Source/UI/Widgets/Button.cpp
    Source/UI/Widgets/Button.h
    Source/UI/Widgets/Image.cpp
    Source/UI/Widgets/Image.h
    Source/UI/Widgets/Label.cpp
    Source/UI/Widgets/Label.h
    Source/UI/Widgets/Panel.cpp
    Source/UI/Widgets/Panel.h
    Source/Windows/Time.cpp
    Source/Windows/File.cpp
    Source/Windows/File.h
)

set(LIBRARIES
    glad
    glfw
    glm
    nlohmann_json
)

set(TARGET_NAME BuildAndDispatch)

add_executable(${TARGET_NAME} ${SOURCES})

target_compile_definitions(${TARGET_NAME} PRIVATE WIN32_LEAN_AND_MEAN WIN32_NO_MIN_MAX)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/stb)
target_link_libraries(${TARGET_NAME} PRIVATE ${LIBRARIES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 23 CXX_EXTENSIONS OFF)
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source FILES ${SOURCES})

