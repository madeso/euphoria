###############################################################################
# render project

set(app_src
    src/render/shader.cc src/render/shader.h
    src/render/opengl_labels.cc src/render/opengl_labels.h
    src/render/opengl_utils.cc src/render/opengl_utils.h
    src/render/texture.cc src/render/texture.h src/render/texture.io.h
    src/render/uniform.cc src/render/uniform.h
    src/render/vertex_layout.cc src/render/vertex_layout.h
    src/render/uniform_buffer.cc src/render/uniform_buffer.h
    src/render/canvas.cc src/render/canvas.h
    src/render/opengl_states.cc src/render/opengl_states.h
    src/render/statechanger.cc src/render/statechanger.h
    
    src/render/rendermode.cc src/render/rendermode.h
    src/render/stencilaction.cc src/render/stencilaction.h
    src/render/cullface.cc src/render/cullface.h
    src/render/compare.cc src/render/compare.h
    src/render/blend.cc src/render/blend.h

    src/render/constants.h
)

source_group("" FILES ${app_src})

set(src
    ${app_src}
)

add_library(render STATIC ${src})
target_precompile_headers(render REUSE_FROM base)

add_library(euph::render ALIAS render)
set_target_properties(render PROPERTIES FOLDER "Libraries")
target_include_directories(render
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(render
    PUBLIC
        euph::assert
        euph::base
        external::opengl
        external::glad
        external::stb_image
        embed::embed
    PRIVATE
        euph::project_options
        euph::project_warnings
)