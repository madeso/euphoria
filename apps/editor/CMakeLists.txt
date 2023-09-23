###############################################################################
# editor project

set(src
    src/editor/browser.cc src/editor/browser.h
    src/editor/main.cc
    src/editor/scimed.cc src/editor/scimed.h
)
source_group("" FILES ${src})

add_executable(editor WIN32 MACOSX_BUNDLE ${src})
set_target_properties(editor PROPERTIES FOLDER "Applications")
target_include_directories(editor
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(editor
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::core
        euph::render
        euph::window
        external::sdl2
        external::imgui
)

