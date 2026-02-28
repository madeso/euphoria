###############################################################################
# editor project

set(src
    src/editor/main.cc
)
source_group("" FILES ${src})

# add_executable(editor WIN32 MACOSX_BUNDLE ${src})
add_executable(editor ${src})
target_precompile_headers(editor REUSE_FROM base)
set_target_properties(editor PROPERTIES FOLDER "Applications")
target_include_directories(editor
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(editor
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::log
        euph::base
        external::sdl2
        external::nuklear
        external::opengl
        external::glad
        external::stb_image
)

