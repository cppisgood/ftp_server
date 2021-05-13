cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(net VERSION 1.0)

add_subdirectory(extern)
add_subdirectory(include)

add_executable(
    servcer.zmm
    server.cpp
)

add_executable(
    cilent.zmm
    cilent.cpp
)

foreach(x servcer.zmm cilent.zmm)
    # target_include_directories(
    #     ${x} PUBLIC
    #     ${PROJECT_SOURCE_DIR}/include
    # )
    target_link_libraries(
        ${x} PRIVATE
        asio
        lib_include
        pthread
    )
    
endforeach(x)


# file(GLOB_RE)
