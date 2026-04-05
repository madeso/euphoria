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
target_precompile_headers(mrgui PRIVATE src/mrgui/pch.private.h )

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


set(data
    ${EUPH_FONTS}/OpenSans-Regular.ttf
    examples/data/uv-texture.png
    examples/data/spritesheet.png
)

embed(embedded
    AS_BINARY
        ${data}
)

set(src
    examples/data/spritesheet.h
    examples/main.cc
    examples/pch.private.h
)

add_executable(mrgui_demo ${src} ${embedded})
source_group("" FILES ${src})
source_group("data" FILES ${data})
source_group("data/generated" FILES ${embedded})
target_precompile_headers(mrgui_demo PRIVATE src/mrgui/pch.private.h )
target_include_directories(mrgui_demo PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(mrgui_demo PROPERTIES FOLDER "Examples/")
target_link_libraries(mrgui_demo PRIVATE euph::project_options
    euph::project_options
    euph::project_warnings
    euph::log
    euph::base
    euph::render
    euph::mrgui
    external::sdl2
    external::opengl
    external::glad
)