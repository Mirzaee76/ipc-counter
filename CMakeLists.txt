cmake_minimum_required(VERSION 3.16)

project(
    ipc_counter
    VERSION 0.0.0
    DESCRIPTION "IPC Counter Example"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ipc_counter
    src/main.cpp
    src/core/cli_parser.cpp
    src/core/application.cpp
    src/ipc/ipc_factory.cpp
    src/ipc/pipe_channel.cpp
    src/ipc/unix_socket_channel.cpp
    src/ipc/shm_channel.cpp
)

target_include_directories(
    ipc_counter
    PRIVATE
        include
        third_party/spdlog/include
)

target_compile_options(
    ipc_counter
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
)

target_link_libraries(
    ipc_counter
    PRIVATE
        pthread
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        ipc_counter
        PRIVATE
            -g
            -O0
    )
endif()