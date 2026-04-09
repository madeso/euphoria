###############################################################################
# render project

set(render_src
    src/eu/render/shader.cc src/eu/render/shader.h
    src/eu/render/opengl_labels.cc src/eu/render/opengl_labels.h
    src/eu/render/opengl_utils.cc src/eu/render/opengl_utils.h
    src/eu/render/texture.cc src/eu/render/texture.h src/eu/render/texture.io.h
    src/eu/render/uniform.cc src/eu/render/uniform.h
    src/eu/render/vertex_layout.cc src/eu/render/vertex_layout.h
    src/eu/render/uniform_buffer.cc src/eu/render/uniform_buffer.h
    src/eu/render/canvas.cc src/eu/render/canvas.h

    src/eu/render/font.cc src/eu/render/font.h
    
    src/eu/render/assets.cc src/eu/render/assets.h
    src/eu/render/camera.cc src/eu/render/camera.h
    src/eu/render/debug.cc src/eu/render/debug.h
    src/eu/render/geom.h
    src/eu/render/fullscreen.cc src/eu/render/fullscreen.h
    src/eu/render/geom.builder.cc src/eu/render/geom.builder.h
    src/eu/render/geom.extract.cc src/eu/render/geom.extract.h
    src/eu/render/linebatch.cc src/eu/render/linebatch.h
    src/eu/render/material.cc src/eu/render/material.h
    src/eu/render/postproc.cc src/eu/render/postproc.h
    src/eu/render/postproc.internal.cc src/eu/render/postproc.internal.h
    src/eu/render/render_settings.cc src/eu/render/render_settings.h
    src/eu/render/renderer.cc src/eu/render/renderer.h
    src/eu/render/renderer.pimpl.cc src/eu/render/renderer.pimpl.h
    src/eu/render/shader.source.cc src/eu/render/shader.source.h
    src/eu/render/shader_resource.cc src/eu/render/shader_resource.h
    src/eu/render/shadow.cc src/eu/render/shadow.h
    src/eu/render/space.cc src/eu/render/space.h
    src/eu/render/state.cc src/eu/render/state.h
    src/eu/render/world.cc src/eu/render/world.h

    src/eu/render/constants.h
    src/eu/render/enable_high_performance_graphics.h

    src/eu/render/pch.private.h
)

source_group("" FILES ${render_src})

set(src
    ${render_src}
)

add_library(render STATIC ${src})
target_precompile_headers(render PRIVATE src/eu/render/pch.private.h)

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
        euph::core
        external::opengl
        external::glad
        external::stb_image
        external::stb_rect_pack
        embed::embed
    PRIVATE
        external::mustache
        euph::project_options
        euph::project_warnings
)


###############################################################################
# test project
set(test_src
    src/eu/render/texture.test.cc
    src/eu/render/uniform_buffer.test.cc
    src/eu/render/vertex_layout.test.cc
)
# set(test_main_src test/main.cc)
source_group("" FILES ${test_src})
add_executable(render_test ${test_src} ${test_main_src})
target_precompile_headers(render_test PRIVATE src/eu/render/pch.private.h)
set_target_properties(render_test PROPERTIES FOLDER "Tests")
target_link_libraries(render_test
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::render
        euph::tests
)
catch_discover_tests(render_test)