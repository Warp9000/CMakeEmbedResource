cmake_minimum_required(VERSION 3.14)
project(CMakeEmbedResource)

set(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CMakeEmbedResource src/main.cpp ${CMAKE_CURRENT_BINARY_DIR}/include/resources.hpp)

include(tools/resource_embedder.cmake)
embed_resources(
    ${CMAKE_CURRENT_BINARY_DIR}/include/resources.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    Resources
    ${CMAKE_PROJECT_NAME}
)
target_include_directories(CMakeEmbedResource PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)