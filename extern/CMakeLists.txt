add_library(
    asio INTERFACE
)
    
# target_sources(
#     asio
#     INTERFACE
#     ${CMAKE_CURRENT_SOURCE_DIR}/asio-1.18.1/include/asio.hpp

# )

# target_compile_options(
#     asio INTERFACE
#     ASIO_STANDALONE
# )
target_include_directories(
    asio
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/asio-1.18.1/include/
)
target_link_libraries(
    asio
    INTERFACE pthread
)