###############################################################################
# editor project

set(fonts
    src/editor/fonts/OpenSans-Regular.ttf
)

embed(embedded
    AS_BINARY
        ${fonts}
)

set(src
    src/editor/main.cc
)
source_group("" FILES ${src})
source_group("fonts" FILES ${fonts})
source_group("fonts/generated" FILES ${embedded})

# add_executable(editor WIN32 MACOSX_BUNDLE ${src})
add_executable(editor ${src} ${embedded})
target_include_directories(editor PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
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
        euph::render
        external::sdl2
        external::nuklear
        external::opengl
        external::glad
        external::stb_image
)

