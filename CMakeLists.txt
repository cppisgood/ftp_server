cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEX2" CACHE STRING "Set C++ Compiler Flags" FORCE)

project(net VERSION 1.0)

add_subdirectory(extern)
add_subdirectory(include)

add_executable(
    ftp_server
    server.cpp
)

# add_executable(
#     cilent.zmm
#     cilent.cpp
# )

# foreach(x ftp_server cilent.zmm)
foreach(x ftp_server)
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
    
endforeach()

# add_executable(
#     tmp.zmm
#     tmp.cpp
# )
# target_link_libraries(
#     tmp.zmm
#     asio
# )


# file(GLOB_RE)
