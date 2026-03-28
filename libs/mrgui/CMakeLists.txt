###############################################################################
# mrgui project

set(mrgui_src
    src/mrgui/widgets.cc src/mrgui/widgets.h

    src/mrgui/pch.private.h
)

source_group("" FILES ${mrgui_src})

set(src
    ${mrgui_src}
)

add_library(mrgui STATIC ${src})
target_precompile_headers(mrgui
    PRIVATE src/mrgui/pch.private.h
)

add_library(euph::mrgui ALIAS mrgui)
set_target_properties(mrgui PROPERTIES FOLDER "Libraries")
target_include_directories(mrgui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(mrgui
    PUBLIC
        euph::assert
        euph::base
        euph::core
        euph::render
        external::stb_image
        embed::embed
        external::sdl2
        external::stb_textedit
    PRIVATE
        euph::project_options
        euph::project_warnings
)