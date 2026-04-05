###############################################################################
# render project

set(render_src
    src/render/shader.cc src/render/shader.h
    src/render/opengl_labels.cc src/render/opengl_labels.h
    src/render/opengl_utils.cc src/render/opengl_utils.h
    src/render/texture.cc src/render/texture.h src/render/texture.io.h
    src/render/uniform.cc src/render/uniform.h
    src/render/vertex_layout.cc src/render/vertex_layout.h
    src/render/uniform_buffer.cc src/render/uniform_buffer.h
    src/render/canvas.cc src/render/canvas.h

    src/render/font.cc src/render/font.h

    src/render/camera.cc src/render/camera.h
    src/render/debug.cc src/render/debug.h
    src/render/fullscreen.cc src/render/fullscreen.h
    src/render/geom.h
    src/render/geom.builder.cc src/render/geom.builder.h
    src/render/geom.extract.cc src/render/geom.extract.h
    src/render/linebatch.cc src/render/linebatch.h
    src/render/material.cc src/render/material.h
    src/render/postproc.cc src/render/postproc.h
    src/render/postproc.internal.cc src/render/postproc.internal.h
    src/render/render_settings.cc src/render/render_settings.h
    src/render/renderer.cc src/render/renderer.h
    src/render/renderer.pimpl.cc src/render/renderer.pimpl.h
    src/render/shader.source.cc src/render/shader.source.h
    src/render/shader_resource.cc src/render/shader_resource.h
    src/render/shadow.cc src/render/shadow.h
    src/render/space.cc src/render/space.h
    src/render/state.cc src/render/state.h
    src/render/world.cc src/render/world.h

    src/render/constants.h
    src/render/enable_high_performance_graphics.h

    src/render/pch.private.h
)

source_group("" FILES ${render_src})

set(src
    ${render_src}
)

add_library(render STATIC ${src})
target_precompile_headers(render PRIVATE src/render/pch.private.h)

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
    src/render/texture.test.cc
    src/render/uniform_buffer.test.cc
    src/render/vertex_layout.test.cc
)
# set(test_main_src test/main.cc)
source_group("" FILES ${test_src})
add_executable(render_test ${test_src} ${test_main_src})
target_precompile_headers(render_test PRIVATE src/render/pch.private.h)
set_target_properties(render_test PROPERTIES FOLDER "Tests")
target_link_libraries(render_test
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::render
        euph::tests
)
catch_discover_tests(render_test)