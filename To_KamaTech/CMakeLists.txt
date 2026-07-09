cmake_minimum_required(VERSION 3.14)
project(kungfu_chess CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

# --- ליבת הפרויקט: כל קבצי המקור (חוץ מ-main.cpp שיש לו main משלו) ---
file(GLOB_RECURSE ALL_SRC CONFIGURE_DEPENDS "src/*.cpp")
list(REMOVE_ITEM ALL_SRC "${CMAKE_SOURCE_DIR}/src/main.cpp")

add_library(chess_lib ${ALL_SRC})
target_include_directories(chess_lib PUBLIC src)

# --- האפליקציה ---
add_executable(kungfu_chess src/main.cpp)
target_link_libraries(kungfu_chess PRIVATE chess_lib)

# --- בדיקות יחידה ---
file(GLOB UNIT_TESTS CONFIGURE_DEPENDS "tests/unit/*.cpp")
add_executable(unit_tests ${UNIT_TESTS})
target_link_libraries(unit_tests PRIVATE chess_lib Catch2::Catch2WithMain)

# --- בדיקות אינטגרציה ---
file(GLOB INTEGRATION_TESTS CONFIGURE_DEPENDS "tests/integration/*.cpp")
add_executable(integration_tests ${INTEGRATION_TESTS})
target_link_libraries(integration_tests PRIVATE chess_lib Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(unit_tests)
catch_discover_tests(integration_tests)