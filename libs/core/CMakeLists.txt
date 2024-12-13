###############################################################################
# core project
set(app_src_cli
    src/core/argparse.cc src/core/argparse.h
    src/core/wordwrap.cc src/core/wordwrap.h
)

set(app_src_replace
    # should only really be used by font packing
    src/core/image.cc src/core/image.h

    # replace with different structure
    src/core/ecs.cc src/core/ecs.h
    src/core/ecs.id.cc src/core/ecs.id.h

    # move to a different input system
    src/core/key.cc src/core/key.h

    # replace with a different scripting language
    src/core/sol_forward.cc src/core/sol_forward.h
    src/core/sol.cc src/core/sol.h

    # replace when updating rendering
    src/core/bufferbuilder2d.cc src/core/bufferbuilder2d.h
    src/core/mesh.cc src/core/mesh.h
    src/core/loadedfont.cc src/core/loadedfont.h
    src/core/texturetypes.cc src/core/texturetypes.h
    src/core/vfs_defaultshaders.cc src/core/vfs_defaultshaders.h
    src/core/vfs_imagegenerator.cc src/core/vfs_imagegenerator.h
    src/core/cache.cc src/core/cache.h

    # only used by image... but is that code used?
    src/core/decompress.cc src/core/decompress.h
)

set(app_src
    src/core/hash.cc src/core/hash.h
    src/core/hash.string.cc src/core/hash.string.h
    
    src/core/table.cc src/core/table.h # used by posson and argparse, merge implementation
    
    src/core/palette.cc src/core/palette.h # simplify

    # merge to a single module
    src/core/sphere.builder.cc src/core/sphere.builder.h
    src/core/sphere.cc src/core/sphere.h
    src/core/sphere.position.cc src/core/sphere.position.h

    src/core/easing.cc src/core/easing.h
    src/core/lines.cc src/core/lines.h
    src/core/poisson.cc src/core/poisson.h
    src/core/polybezier.cc src/core/polybezier.h # complete
    src/core/torcurve.cc src/core/torcurve.h
    
    src/core/camera3.cc src/core/camera3.h
    src/core/camera3.editor.cc src/core/camera3.editor.h
    src/core/fpscontroller.cc src/core/fpscontroller.h

    src/core/canvaslogic.cc src/core/canvaslogic.h
    src/core/collisionmesh.cc src/core/collisionmesh.h
    src/core/compiler_warning.cc src/core/compiler_warning.h
    src/core/dump.cc src/core/dump.h
    src/core/enum.cc src/core/enum.h
    src/core/guid.cc src/core/guid.h
    src/core/helptexthover.cc src/core/helptexthover.h
    src/core/tweenable.cc src/core/tweenable.h
    src/core/intersection.cc src/core/intersection.h
    src/core/iterate.cc src/core/iterate.h
    
    src/core/mousebehaviour.cc src/core/mousebehaviour.h # move to input
    src/core/sensitivity.cc src/core/sensitivity.h # replace with a float
    src/core/rng.cc src/core/rng.h
    src/core/shufflebag.cc src/core/shufflebag.h
    
    src/core/stdutils.cc src/core/stdutils.h
    src/core/tablelayout.cc src/core/tablelayout.h
    src/core/textfileparser.cc src/core/textfileparser.h
    src/core/ui_text.cc src/core/ui_text.h
    src/core/utf8.cc src/core/utf8.h
    
    src/core/viewport.cc src/core/viewport.h
    src/core/viewportdef.cc src/core/viewportdef.h
)

source_group("" FILES ${app_src})
source_group("cli" FILES ${app_src_cli})
source_group("replace" FILES ${app_src_replace})

set(src
    ${app_src}
    ${app_src_cli}
    ${app_src_replace}
)


add_library(core STATIC ${src})
add_library(euph::core ALIAS core)
target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(core
    PUBLIC
        euph::log
        euph::assert
        euph::fmt
        external::lua
        undef_windows::undef_windows
        euph::files
        euph::base
    PRIVATE
        euph::project_options
        euph::project_warnings
        external::crc32
        external::assimp
        external::stb_image
        external::stb_image_write
        external::stb_rect_pack
        external::stb_truetype
        external::font8x8
        external::font8x13
        external::guid
)
set_target_properties(core PROPERTIES FOLDER "Libraries")

# if(NOT GENERATE_COVERAGE)
#     generate_header_test(core
#         INCLUDE_BASE "${CMAKE_CURRENT_SOURCE_DIR}/src"
#         FILES ${src}
#     )
# endif()

###############################################################################
# test project

set(test_src
    data/test/test_image.png
    src/core/argparse.test.cc
    src/core/bufferbuilder2d.test.cc
    src/core/camera.test.cc
    src/core/ecs.test.cc
    src/core/guid.test.cc
    src/core/hash.test.cc
    src/core/image.test.cc
    src/core/tweenable.test.cc
    src/core/intersection.test.cc
    src/core/iterate.test.cc
    src/core/loadedfont.test.cc
    src/core/table.test.cc
    src/core/tablelayout.test.cc
    src/core/textfileparser.test.cc
    src/core/torcurve.test.cc
    src/core/ui_text.test.cc
    src/core/utf8.test.cc
    src/core/viewportdef.test.cc
    src/core/wordwrap.test.cc
)
source_group("" FILES ${test_src})
add_executable(core_test ${test_src})
set_target_properties(core_test PROPERTIES FOLDER "Tests")
target_link_libraries(core_test
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::core
        euph::tests
)
catch_discover_tests(core_test)

###############################################################################
# example projects

file(GLOB all_examples_src examples/*.cc)
source_group("" FILES ${all_examples_src})

# examples applications that serve no purpose other than
# testing the libraries and perhaps generate some pretty visuals :)

add_executable(examples-rng examples/rng_main.cc)
set_target_properties(examples-rng PROPERTIES FOLDER "Examples/Core")
target_link_libraries(examples-rng PRIVATE euph::project_options euph::project_warnings euph::core)

add_executable(examples-coldet examples/coldet.cc)
set_target_properties(examples-coldet PROPERTIES FOLDER "Examples/Core")
target_link_libraries(examples-coldet PRIVATE euph::project_options euph::project_warnings euph::core)

add_executable(examples-svg-dump examples/svg_dump.cc)
set_target_properties(examples-svg-dump PROPERTIES FOLDER "Examples/Core")
target_link_libraries(examples-svg-dump PRIVATE euph::project_options euph::project_warnings euph::core)

add_executable(examples-three-dump examples/three_dump.cc)
set_target_properties(examples-three-dump PROPERTIES FOLDER "Examples/Core")
target_link_libraries(examples-three-dump PRIVATE euph::project_options euph::project_warnings euph::core)
