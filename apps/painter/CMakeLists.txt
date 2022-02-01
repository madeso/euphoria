###############################################################################
# painter project

set(src
    src/painter/main.cc
)
source_group("" FILES ${src})

add_executable(painter WIN32 MACOSX_BUNDLE ${src})
set_target_properties(painter PROPERTIES FOLDER "Applications")
target_include_directories(painter
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(painter
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::core
        euph::render
        euph::window
        external::imgui
        external::sdl2
)

