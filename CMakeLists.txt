cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(net VERSION 1.0)

add_subdirectory(extern)

add_executable(${PROJECT_NAME} tmp.cpp)

# file(GLOB_RE)

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(
    ${PROJECT_NAME} PRIVATE asio
)