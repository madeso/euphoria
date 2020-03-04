file(GLOB app_src *.cc;*.h;*.inl)
set(app_src
    aabb.cc aabb.h
    angle.cc angle.h
    argparse.cc argparse.h
    assert.cc assert.h
    axisangle.cc axisangle.h
    base64.cc base64.h
    bezierpath.cc bezierpath.h
    bitfield.h
    bufferbuilder2d.cc bufferbuilder2d.h
    cache.h camera.cc
    camera.h
    canvaslogic.cc canvaslogic.h
    chatbot.cc chatbot.h
    cint.cc cint.h
    cli_position.cc cli_position.h
    cli_progress_bar.cc cli_progress_bar.h
    cli_progress_dots.cc cli_progress_dots.h
    closestpoint.h
    colorbrewer.cc colorbrewer.h
    colors.cc colors.h
    console.cc console.h
    cp437.cc cp437.h
    cpp.h
    crc32.cc crc32.h
    ctree.cc ctree.h
    decompress.cc decompress.h
    dump.cc dump.h
    ecs-id.h
    ecs-systems.cc ecs-systems.h
    ecs.cc ecs.h
    editdistance.cc editdistance.h
    enum.cc enum.h
    enumtostring.h
    findstring.cc findstring.h
    fonts.cc fonts.h
    fpscontroller.cc fpscontroller.h
    functional.h
    generator_cell.cc generator_cell.h
    generator_maze.cc generator_maze.h
    hash_fnv1a.cc hash_fnv1a.h
    history.h
    id.cc id.h
    identicon.cc identicon.h
    image.cc image.h
    image_canvas.cc image_canvas.h
    image_draw.cc image_draw.h
    imagefilters.cc imagefilters.h
    imageops.cc imageops.h
    insertionsort.h
    interpolate.cc interpolate.h
    intersection.cc intersection.h
    ints.h
    io.cc io.h
    key.cc key.h
    knuthshuffle.h
    lines.cc lines.h
    log.cc log.h
    markov.cc markov.h
    mat2.h
    mat3.h
    mat4.h
    memorychunk.cc memorychunk.h
    mesh.cc mesh.h
    minmax.h
    multisort.h
    noise_simplex.cc noise_simplex.h
    noncopyable.h
    numeric.cc numeric.h
    orbitcontroller.cc orbitcontroller.h
    os.cc os.h
    palette.cc palette.h
    palette_all.cc palette_all.h
    palette_cubehelix.cc palette_cubehelix.h
    palette_lospec.cc palette_lospec.h
    palette_matplot.cc palette_matplot.h
    palette_mycarta.cc palette_mycarta.h
    palette_tableu.cc palette_tableu.h
    palette_wes.cc palette_wes.h
    plane.cc plane.h
    poisson.cc poisson.h
    polarcoord.cc polarcoord.h
    propertytree.cc propertytree.h
    proto.cc proto.h
    quat.h
    quicksort.h
    radix.h
    random.cc random.h
    range.cc range.h
    ray.cc ray.h
    raytracer.cc raytracer.h
    rect.h
    rgb.cc rgb.h
    rgb_blend.cc rgb_blend.h
    sensitivity.cc sensitivity.h
    shufflebag.h
    size.h
    sphere.cc sphere.h
    stdutils.h
    str.cc str.h
    stringmerger.cc stringmerger.h
    stringtable.cc stringtable.h
    stringutils.cc stringutils.h
    table.h
    tablelayout.h
    textbox.cc textbox.h
    textfileparser.cc textfileparser.h
    texttemplate.cc texttemplate.h
    texturetypes.cc texturetypes.h
    timepoint.cc timepoint.h
    tracery.cc tracery.h
    typeinfo.cc typeinfo.h
    ui_text.cc ui_text.h
    undef_windows.h
    utf8.h
    vec2.cc vec2.h
    vec3.h
    vec4.h
    vfs.cc vfs.h
    vfs_defaultshaders.cc vfs_defaultshaders.h
    vfs_imagegenerator.cc vfs_imagegenerator.h
    vfs_path.cc vfs_path.h
    viewport.cc viewport.h
)

set(src ${app_src})
source_group("" FILES ${src})


include_directories(SYSTEM ${assimp_INCLUDE_DIRS})



# todo: move image loading to a io library instead
include_directories(SYSTEM ../external/stb)


# freetype
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})


# release date 2017-09-23 version duktape-2.2.0.tar.xz
include_directories(SYSTEM ../external/duktape)
set(src ${src} ../external/duktape/duktape/duktape.c)



include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
SET(Gaf_CUSTOM_PREFIX gaf_)
GAF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
                      ../file_types/enum.gaf
                      ../file_types/mesh.gaf
                      ../file_types/chatbot.gaf
                      )
set(src ${src} ${PROTO_SRCS} ${PROTO_HDRS})



include_directories(../external/aheasing)
set(src_aheasing
    ../external/aheasing/aheasing/easing.c
    ../external/aheasing/aheasing/easing.h
    )
set(src ${src} ${src_aheasing})
source_group("AHEasing" FILES ${src_aheasing})


include_directories(../external/font8x8/)
include_directories(../external/font8x13/)

add_library(core STATIC ${src})
target_link_libraries(core
    ${assimp_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    )
