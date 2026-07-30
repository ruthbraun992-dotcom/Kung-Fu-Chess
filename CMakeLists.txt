cmake_minimum_required(VERSION 3.14)

project(kungfu_chess CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==========================================================
# Dependencies
# ==========================================================

add_subdirectory(ThirdParty/Catch2)


# ==========================================================
# SQLite
# ==========================================================

add_library(sqlite3_lib STATIC
    ${CMAKE_SOURCE_DIR}/ThirdParty/sqlite3/sqlite3.c
)

target_include_directories(sqlite3_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/ThirdParty/sqlite3
)


# ==========================================================
# Chess Logic Library
# ==========================================================

file(GLOB_RECURSE LOGIC_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/Server/Logic/*.cpp
    Common/*.cpp
)

add_library(chess_lib
    ${LOGIC_SOURCES}
)

target_include_directories(chess_lib PUBLIC

    ${CMAKE_SOURCE_DIR}/Server/Logic
    ${CMAKE_SOURCE_DIR}/Common

    ${CMAKE_SOURCE_DIR}/Server/Logic/rules
    ${CMAKE_SOURCE_DIR}/Server/Logic/engine
    ${CMAKE_SOURCE_DIR}/Server/Logic/realtime
    ${CMAKE_SOURCE_DIR}/Server/Logic/stats
    ${CMAKE_SOURCE_DIR}/Server/Logic/animation
    ${CMAKE_SOURCE_DIR}/Server/Logic/events

    ${CMAKE_SOURCE_DIR}/Server

    ${CMAKE_SOURCE_DIR}/asio/asio/include
    ${CMAKE_SOURCE_DIR}/websocketpp

    ${CMAKE_SOURCE_DIR}/ThirdParty/json/include
)

target_compile_definitions(chess_lib PUBLIC
    ASIO_STANDALONE
    _WEBSOCKETPP_CPP11_STL_
)

target_link_libraries(chess_lib PRIVATE
    sqlite3_lib
)


# ==========================================================
# Server Application
# ==========================================================

file(GLOB SERVER_SOURCES CONFIGURE_DEPENDS
    Server/*.cpp
)

# file(GLOB_RECURSE LOGIC_SOURCES
#     Server/Logic/**/*.cpp
# )
add_executable(server_app 
    Server/main.cpp
)

target_include_directories(server_app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Server/Logic
    ${CMAKE_CURRENT_SOURCE_DIR}/Common
)

target_include_directories(server_app PRIVATE
    ${CMAKE_SOURCE_DIR}
)

target_compile_definitions(server_app PRIVATE
    ASIO_STANDALONE
)

target_link_libraries(server_app PRIVATE
    chess_lib
    sqlite3_lib
)


# ==========================================================
# Tests
# ==========================================================

enable_testing()

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    tests/unit/*.cpp
    tests/integration/*.cpp
)

add_executable(unit_tests
    ${TEST_SOURCES}

    Server/WebSocketServer.cpp
    Server/SessionManager.cpp
    Server/UserRepository.cpp
    Server/PasswordHasher.cpp
    Server/GameManager.cpp
    Server/MatchmakingQueue.cpp
    Server/RoomManager.cpp
)

target_include_directories(unit_tests PRIVATE

    ${CMAKE_SOURCE_DIR}

    # ${CMAKE_SOURCE_DIR}/Logic
    ${CMAKE_SOURCE_DIR}/Common

    ${CMAKE_SOURCE_DIR}/Server

    ${CMAKE_SOURCE_DIR}/ThirdParty/sqlite3
)

target_compile_definitions(unit_tests PRIVATE
    ASIO_STANDALONE
)

target_link_libraries(unit_tests PRIVATE

    chess_lib

    sqlite3_lib

    Catch2::Catch2WithMain
)


include(${CMAKE_SOURCE_DIR}/ThirdParty/Catch2/extras/Catch.cmake)

catch_discover_tests(unit_tests)



# ==========================================================
# UI
# ==========================================================

add_subdirectory(UI)



# ==========================================================
# Warnings
# ==========================================================

if(MSVC)

    target_compile_options(chess_lib PRIVATE /W4)

    target_compile_options(server_app PRIVATE /W4)

    target_compile_options(unit_tests PRIVATE /W4)

endif()