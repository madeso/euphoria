###############################################################################
# pixel project

set(src
    src/pixel/main.cc
)
source_group("" FILES ${src})

add_executable(pixel WIN32 MACOSX_BUNDLE ${src})
set_target_properties(pixel PROPERTIES FOLDER "Applications")
target_include_directories(pixel
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(pixel
    core
    render
    window
    imgui
    sdl2
)

