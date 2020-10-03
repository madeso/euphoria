set(app_src
    actor.cc actor.h
    attributebinder.cc attributebinder.h
    buffer.cc buffer.h
    buffer2d.cc buffer2d.h
    compiledlines.cc compiledlines.h
    compiledmesh.cc compiledmesh.h
    debuggl.cc debuggl.h
    defaultfiles.cc defaultfiles.h
    fontcache.cc fontcache.h
    font.cc font.h
    gl.h
    gltypes.h
    id.cc id.h
    init.cc init.h
    instance.cc instance.h
    light.cc light.h
    materialshader.cc materialshader.h
    materialshadercache.h
    positionedlines.cc positionedlines.h
    scalablesprite.cc scalablesprite.h
    shader.cc shader.h
    shaderattribute.cc shaderattribute.h
    shaderattribute2d.cc shaderattribute2d.h
    shaderattribute3d.cc shaderattribute3d.h
    shaderuniform.cc shaderuniform.h
    sprite.cc sprite.h
    spritebatch.cc spritebatch.h
    spriterender.cc spriterender.h
    texture.cc texture.h
    texturecache.cc texturecache.h
    viewport.cc viewport.h
    viewporthandler.cc viewporthandler.h
    world.cc world.h
)
set(src ${app_src})
source_group("" FILES ${app_src})

add_library(render STATIC ${src})

target_link_libraries(render
    PRIVATE
    core
    glad
    opengl
    file_types
    stb_rect_pack
)

