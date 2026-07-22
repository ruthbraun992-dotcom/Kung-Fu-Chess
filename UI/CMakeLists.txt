project(kungfu_ui)

set(OPENCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/OpenCV_451")
set(OPENCV_INCLUDE_DIR "${OPENCV_DIR}/include")
set(OPENCV_LIB_DIR "${OPENCV_DIR}/bin")

file(GLOB_RECURSE UI_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(kungfu_chess
    ${UI_SRC}
)

target_include_directories(kungfu_chess PRIVATE
    ${OPENCV_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/Logic
    ${CMAKE_SOURCE_DIR}/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_directories(kungfu_chess PRIVATE
    ${OPENCV_LIB_DIR}
)

target_link_libraries(kungfu_chess PRIVATE
    chess_lib
    $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.lib>
    $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.lib>
)

add_custom_command(TARGET kungfu_chess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<$<CONFIG:Debug>:${OPENCV_LIB_DIR}/opencv_world451d.dll>
        $<$<CONFIG:Release>:${OPENCV_LIB_DIR}/opencv_world451.dll>
        $<TARGET_FILE_DIR:kungfu_chess>
)