cmake_minimum_required(VERSION 3.14)

project(kungfu_chess CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==========================================================
# Chess library
# ==========================================================

file(GLOB_RECURSE LOGIC_SRC CONFIGURE_DEPENDS
    Logic/*.cpp
    Common/*.cpp
)

add_library(chess_lib ${LOGIC_SRC})

target_include_directories(chess_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/Logic
    ${CMAKE_SOURCE_DIR}/Common
     ${CMAKE_SOURCE_DIR}/Logic/rules
    ${CMAKE_SOURCE_DIR}/Logic/engine
    ${CMAKE_SOURCE_DIR}/Logic/realtime
    ${CMAKE_SOURCE_DIR}/Logic/stats
    ${CMAKE_SOURCE_DIR}/Logic/animation
    ${CMAKE_SOURCE_DIR}/Logic/events
)

# ==========================================================
# Catch2
# ==========================================================

add_subdirectory(ThirdParty/Catch2)

# ==========================================================
# Unit Tests
# ==========================================================

enable_testing()

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    tests/unit/*.cpp
    tests/integration/*.cpp
)

add_executable(unit_tests
    ${TEST_SOURCES}
)



target_link_libraries(unit_tests PRIVATE
    chess_lib
    Catch2::Catch2WithMain
)

include(${CMAKE_SOURCE_DIR}/ThirdParty/Catch2/extras/Catch.cmake)
catch_discover_tests(unit_tests)

# ==========================================================
# UI
# ==========================================================

add_subdirectory(UI)

# ==========================================================
# Compiler warnings
# ==========================================================

if(MSVC)
    target_compile_options(chess_lib PRIVATE /W4)
    target_compile_options(unit_tests PRIVATE /W4)
endif()