###############################################################################
# core project

set(app_src_color
    src/core/colorbrewer.cc src/core/colorbrewer.h
    src/core/colors.cc src/core/colors.h
    src/core/image.cc src/core/image.h
    src/core/image_canvas.cc src/core/image_canvas.h
    src/core/image_draw.cc src/core/image_draw.h
    src/core/imagefilters.cc src/core/imagefilters.h
    src/core/imageops.cc src/core/imageops.h
    src/core/palette.cc src/core/palette.h
    src/core/palette_extract.cc src/core/palette_extract.h
    src/core/palette_all.cc src/core/palette_all.h
    src/core/palette_cubehelix.cc src/core/palette_cubehelix.h
    src/core/palette_lospec.cc src/core/palette_lospec.h
    src/core/palette_matplot.cc src/core/palette_matplot.h
    src/core/palette_mycarta.cc src/core/palette_mycarta.h
    src/core/palette_tableu.cc src/core/palette_tableu.h
    src/core/palette_wes.cc src/core/palette_wes.h
)

set(app_src_math
    src/core/bezierpath.cc src/core/bezierpath.h
    src/core/aabb.cc src/core/aabb.h
    src/core/angle.cc src/core/angle.h
    src/core/axisangle.cc src/core/axisangle.h
    src/core/lines.cc src/core/lines.h
    src/core/mat2.cc src/core/mat2.h
    src/core/mat3.cc src/core/mat3.h
    src/core/mat4.cc src/core/mat4.h
    src/core/pack.cc src/core/pack.h
    src/core/collage.cc src/core/collage.h
    src/core/plane.cc src/core/plane.h
    src/core/poisson.cc src/core/poisson.h
    src/core/polarcoord.cc src/core/polarcoord.h
    src/core/ray.cc src/core/ray.h
    src/core/rect.h
    src/core/rgb.cc src/core/rgb.h
    src/core/rgb_blend.cc src/core/rgb_blend.h
    src/core/sphere.cc src/core/sphere.h
    src/core/vec2.cc src/core/vec2.h
    src/core/vec3.h
    src/core/vec4.h
)

set(app_src_cli
    src/core/cli_progress_dots.cc src/core/cli_progress_dots.h
    src/core/cp437.cc src/core/cp437.h
    src/core/ctree.cc src/core/ctree.h
    src/core/textbox.cc src/core/textbox.h
)

set(app_src_pcg
    src/core/generator_cell.cc src/core/generator_cell.h
    src/core/generator_maze.cc src/core/generator_maze.h
    src/core/markov.cc src/core/markov.h
    src/core/noise_simplex.cc src/core/noise_simplex.h
    src/core/tracery.cc src/core/tracery.h
    src/core/drunken_bishop.cc src/core/drunken_bishop.h
)

set(app_src_hash
    src/core/hash_fnv1a.cc src/core/hash_fnv1a.h
    src/core/hashgen_retro.cc src/core/hashgen_retro.h
    src/core/hashgen_sprator.cc src/core/hashgen_sprator.h
    src/core/identicon.cc src/core/identicon.h
)

set(app_src_table
    src/core/fourway.cc src/core/fourway.h
    src/core/table_string.cc src/core/table_string.h
    src/core/table_bool.cc src/core/table_bool.h
    src/core/table.h
)

set(app_src
    src/core/result.cc src/core/result.h
    src/core/custom_parser.cc src/core/custom_parser.h
    src/core/default_parse.cc src/core/default_parse.h
    src/core/argparse.cc src/core/argparse.h
    src/core/assert.cc src/core/assert.h
    src/core/base64.cc src/core/base64.h
    src/core/bitfield.h
    src/core/bufferbuilder2d.cc src/core/bufferbuilder2d.h
    src/core/cache.h
    src/core/camera.cc src/core/camera.h
    src/core/canvaslogic.cc src/core/canvaslogic.h
    src/core/chatbot.cc src/core/chatbot.h
    src/core/cint.cc src/core/cint.h
    src/core/closestpoint.h
    src/core/console.cc src/core/console.h
    src/core/cypher.cc src/core/cypher.h
    src/core/cpp.h
    src/core/crc32.cc src/core/crc32.h
    src/core/decompress.cc src/core/decompress.h
    src/core/datetime.cc src/core/datetime.h
    src/core/dump.cc src/core/dump.h
    src/core/ecs-id.h
    src/core/ecs-systems.cc src/core/ecs-systems.h
    src/core/ecs.cc src/core/ecs.h
    src/core/editdistance.cc src/core/editdistance.h
    src/core/enum.cc src/core/enum.h
    src/core/enumtostring.h
    src/core/findstring.cc src/core/findstring.h
    src/core/fpscontroller.cc src/core/fpscontroller.h
    src/core/functional.h
    src/core/history.h
    src/core/id.cc src/core/id.h
    src/core/insertionsort.h
    src/core/interpolate.cc src/core/interpolate.h
    src/core/intersection.cc src/core/intersection.h
    src/core/ints.h
    src/core/io.cc src/core/io.h
    src/core/key.cc src/core/key.h
    src/core/knuthshuffle.h
    src/core/loadedfont.cc src/core/loadedfont.h
    src/core/log.cc src/core/log.h
    src/core/memorychunk.cc src/core/memorychunk.h
    src/core/mesh.cc src/core/mesh.h
    src/core/minmax.h
    src/core/multisort.h
    src/core/nlp_line.cc src/core/nlp_line.h
    src/core/nlp_sentance.cc src/core/nlp_sentance.h
    src/core/noncopyable.h
    src/core/numeric.cc src/core/numeric.h
    src/core/orbitcontroller.cc src/core/orbitcontroller.h
    src/core/os.cc src/core/os.h
    src/core/propertytree.cc src/core/propertytree.h
    src/core/proto.cc src/core/proto.h
    src/core/quat.h
    src/core/quicksort.h
    src/core/radix.h
    src/core/random.cc src/core/random.h
    src/core/rng.cc src/core/rng.h
    src/core/range.cc src/core/range.h
    src/core/raytracer.cc src/core/raytracer.h
    src/core/sensitivity.cc src/core/sensitivity.h
    src/core/shufflebag.h
    src/core/size.h
    src/core/stdutils.h
    src/core/str.cc src/core/str.h
    src/core/stringmerger.cc src/core/stringmerger.h
    src/core/stringutils.cc src/core/stringutils.h
    src/core/subvec.cc src/core/subvec.h
    src/core/tablelayout.h
    src/core/textfileparser.cc src/core/textfileparser.h
    src/core/texttemplate.cc src/core/texttemplate.h
    src/core/texturetypes.cc src/core/texturetypes.h
    src/core/timepoint.cc src/core/timepoint.h
    src/core/typeinfo.cc src/core/typeinfo.h
    src/core/ui_text.cc src/core/ui_text.h
    src/core/undef_windows.h
    src/core/utf8.h
    src/core/vfs.cc src/core/vfs.h
    src/core/vfs_defaultshaders.cc src/core/vfs_defaultshaders.h
    src/core/vfs_imagegenerator.cc src/core/vfs_imagegenerator.h
    src/core/vfs_path.cc src/core/vfs_path.h
    src/core/viewportdef.cc src/core/viewportdef.h
    src/core/wordwrap.cc src/core/wordwrap.h
)

source_group("" FILES ${app_src})
source_group("color" FILES ${app_src_color})
source_group("math" FILES ${app_src_math})
source_group("cli" FILES ${app_src_cli})
source_group("pcg" FILES ${app_src_pcg})
source_group("hash" FILES ${app_src_hash})
source_group("table" FILES ${app_src_table})

set(src
    ${app_src}
    ${app_src_color}
    ${app_src_math}
    ${app_src_cli}
    ${app_src_pcg}
    ${app_src_hash}
    ${app_src_table}
)


add_library(core STATIC ${src})
target_include_directories(core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(core
    PUBLIC
    magic_enum
    spdlog
    rapidjson
    PRIVATE
    stb_image stb_image_write stb_rect_pack
    font8x8 font8x13 aheasing duktape file_types range
    assimp freetype
)


###############################################################################
# test project

set(test_src
    src/core/angle.test.cc
    src/core/argparse.test.cc
    src/core/axisangle.test.cc
    src/core/base64.test.cc
    src/core/bitfield.test.cc
    src/core/bufferbuilder2d.test.cc
    src/core/camera.test.cc
    src/core/chatbot.test.cc
    src/core/cypher.test.cc
    src/core/commandlineparser.test.cc
    src/core/date.test.cc
    src/core/drunken_bishop.test.cc
    src/core/editdistance.test.cc
    src/core/enumtostring.test.cc
    src/core/findstring.test.cc
    src/core/fnv1a.test.cc
    src/core/image.test.cc
    data/test/test_image.png
    src/core/insertionsort.test.cc
    src/core/interpolate.test.cc
    src/core/loadedfont.test.cc
    src/core/mat4.test.cc
    src/core/multisort.test.cc
    src/core/nlp_line.test.cc
    src/core/nlp_sentance.test.cc
    src/core/numeric.test.cc
    src/core/pack.test.cc
    src/core/quat.test.cc
    src/core/quicksort.test.cc
    src/core/radix.test.cc
    src/core/range.test.cc
    src/core/rect.test.cc
    src/core/rgb.test.cc
    src/core/rgb_blend.test.cc
    src/core/stringmerger.test.cc
    src/core/stringtable.test.cc
    src/core/stringutils.test.cc
    src/core/subvec.test.cc
    src/core/table.test.cc
    src/core/tablelayout.test.cc
    src/core/template.test.cc
    src/core/textbox.test.cc
    src/core/textfileparser.test.cc
    src/core/tracery.test.cc
    src/core/ui_text.test.cc
    src/core/utf8.test.cc
    src/core/vec2.test.cc
    src/core/vec3.test.cc
    src/core/vec4.test.cc
    src/core/vfs.test.cc
    src/core/vfspath.test.cc
    src/core/viewportdef.test.cc
    src/core/wordwrap.test.cc
)
set(test_main_src test/main.cc)
source_group("" FILES ${test_src})
source_group("main" FILES ${test_main_src})
add_executable(core_test ${test_src} ${test_main_src})
target_link_libraries(core_test PUBLIC core tests)


###############################################################################
# example projects

file(GLOB all_examples_src examples/*.cc)
source_group("" FILES ${all_examples_src})

# single file applications that could be useful

add_executable(palextract examples/palette_extract.cc)
target_link_libraries(palextract core)

add_executable(collage examples/collage.cc)
target_link_libraries(collage core)

add_executable(tabletool examples/tabletool.cc)
target_link_libraries(tabletool core)

add_executable(base64 examples/base64.cc)
target_link_libraries(base64 core)

add_executable(markov examples/markov_main.cc)
target_link_libraries(markov core)

add_executable(tracery examples/tracery.cc)
target_link_libraries(tracery core)

add_executable(raytracer examples/raytracer_main.cc)
target_link_libraries(raytracer core)

add_executable(cypher examples/cypher_main.cc)
target_link_libraries(cypher core)


# examples applications that serve no purpose other than
# testing the libraries and perhaps generate some pretty visuals :)

add_executable(examples-drunken-bishop examples/drunken_bishop_main.cc)
target_link_libraries(examples-drunken-bishop core)

add_executable(examples-rng examples/rng_main.cc)
target_link_libraries(examples-rng core)

add_executable(examples-font examples/font_main.cc)
target_link_libraries(examples-font core)

add_executable(examples-palette examples/palette_main.cc)
target_link_libraries(examples-palette core)

add_executable(examples-chatbot examples/chatbot.cc)
target_link_libraries(examples-chatbot core)

add_executable(examples-coldet examples/coldet.cc)
target_link_libraries(examples-coldet core)

add_executable(examples-draw-test examples/draw_test.cc)
target_link_libraries(examples-draw-test core)

add_executable(examples-img examples/img.cc)
target_link_libraries(examples-img core)

add_executable(examples-poisson-test examples/poisson_test.cc)
target_link_libraries(examples-poisson-test core)

add_executable(examples-svg-dump examples/svg_dump.cc)
target_link_libraries(examples-svg-dump core)

add_executable(examples-three-dump examples/three_dump.cc)
target_link_libraries(examples-three-dump core)

add_executable(examples-identicon examples/identicon_main.cc)
target_link_libraries(examples-identicon core)

add_executable(examples-canvas examples/canvas_main.cc)
target_link_libraries(examples-canvas core)

add_executable(examples-tree examples/tree.cc)
target_link_libraries(examples-tree core)

add_executable(examples-worldgen examples/worldgen.cc)
target_link_libraries(examples-worldgen core)

add_executable(examples-voronoi examples/voronoi.cc)
target_link_libraries(examples-voronoi core)
