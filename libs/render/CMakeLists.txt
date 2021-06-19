###############################################################################
# render project

set(app_src
    src/render/actor.cc src/render/actor.h
    src/render/attributebinder.cc src/render/attributebinder.h
    src/render/buffer.cc src/render/buffer.h
    src/render/buffer2d.cc src/render/buffer2d.h
    src/render/compiledlines.cc src/render/compiledlines.h
    src/render/compiledmesh.cc src/render/compiledmesh.h
    src/render/debuggl.cc src/render/debuggl.h
    src/render/defaultfiles.cc src/render/defaultfiles.h
    src/render/fontcache.cc src/render/fontcache.h
    src/render/font.cc src/render/font.h
    src/render/gl.h src/render/gl.cc
    src/render/gltypes.h src/render/gltypes.cc
    src/render/id.cc src/render/id.h
    src/render/init.cc src/render/init.h
    src/render/instance.cc src/render/instance.h
    src/render/light.cc src/render/light.h
    src/render/materialshader.cc src/render/materialshader.h
    src/render/materialshadercache.h src/render/materialshadercache.cc
    src/render/positionedlines.cc src/render/positionedlines.h
    src/render/scalablesprite.cc src/render/scalablesprite.h
    src/render/shader.cc src/render/shader.h
    src/render/shaderattribute.cc src/render/shaderattribute.h
    src/render/shaderattribute2d.cc src/render/shaderattribute2d.h
    src/render/shaderattribute3d.cc src/render/shaderattribute3d.h
    src/render/shaderuniform.cc src/render/shaderuniform.h
    src/render/sprite.cc src/render/sprite.h
    src/render/spritebatch.cc src/render/spritebatch.h
    src/render/spriterender.cc src/render/spriterender.h
    src/render/texture.cc src/render/texture.h
    src/render/texturecache.cc src/render/texturecache.h
    src/render/viewport.cc src/render/viewport.h
    src/render/viewporthandler.cc src/render/viewporthandler.h
    src/render/world.cc src/render/world.h
)
set(src ${app_src})
source_group("" FILES ${app_src})

add_library(render STATIC ${src})
set_target_properties(render PROPERTIES FOLDER "Libraries")
target_include_directories(render
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(render
    PRIVATE
        project_options
        project_warnings
        core
        glad
        opengl
        file_types
        stb_rect_pack
)


###############################################################################
# example projects

file(GLOB all_examples_src examples/*.cc)
source_group("" FILES ${all_examples_src})

add_executable(examples-3d WIN32 MACOSX_BUNDLE examples/3d.cc)
set_target_properties(examples-3d PROPERTIES FOLDER "Examples/Render")
target_link_libraries(examples-3d
    PRIVATE
        project_options
        project_warningscore
        render
        window
        imgui
        sdl2
)

add_executable(examples-2d WIN32 MACOSX_BUNDLE examples/2d.cc)
set_target_properties(examples-2d PROPERTIES FOLDER "Examples/Render")
target_link_libraries(examples-2d
    PRIVATE
        project_options
        project_warnings
        core
        render
        window
        imgui
        sdl2
)
