cmake_minimum_required(VERSION 3.29)

set(CMAKE_CXX_FLAGS_DEBUG "-Wextra -Wall -Wpedantic -g")
set(CMAKE_CXX_FLAGS_RELEASE "-Wextra -Wall -Wpedantic")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(
  falling_sand
  VERSION 0.0.1
  LANGUAGES CXX # To build `falling_sand`
            C # To build `glad`
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# glad
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC include)

# Project
add_executable(${PROJECT_NAME})

# Define DEBUG_BUILD if CMAKE_BUILD_TYPE is Debug
target_compile_definitions(${PROJECT_NAME}
                           PRIVATE $<$<CONFIG:Debug>:DEBUG_BUILD=1>)

file(GLOB SOURCES src/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad)
