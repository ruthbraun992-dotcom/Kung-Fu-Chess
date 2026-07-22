cmake_minimum_required(VERSION 3.14)

project(kungfu_chess CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(GLOB_RECURSE LOGIC_SRC CONFIGURE_DEPENDS
    Logic/*.cpp
    Common/*.cpp
)

add_library(chess_lib ${LOGIC_SRC})

target_include_directories(chess_lib PUBLIC
    Logic
    Common
)

target_include_directories(chess_lib PUBLIC
    Logic
    Common
    ${CMAKE_SOURCE_DIR}/ThirdParty
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(UI)



if(MSVC)
    add_compile_options(/W4)
endif()