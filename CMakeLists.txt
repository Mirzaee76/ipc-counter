cmake_minimum_required(VERSION 3.16)

project(
    counter_ipc
    VERSION 0.0.0
    DESCRIPTION "IPC Counter Example"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(counter_ipc
    src/main.cpp
    src/core/cli_parser.cpp
    src/core/application.cpp
    src/ipc/ipc_factory.cpp
    src/ipc/pipe_channel.cpp
)

target_include_directories(
    counter_ipc
    PRIVATE
        include
        third_party/spdlog/include
)

target_compile_options(
    counter_ipc
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        counter_ipc
        PRIVATE
            -g
            -O0
    )
endif()