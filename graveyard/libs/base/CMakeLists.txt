###############################################################################
# base project

set(app_src
    src/base/aabb.cc src/base/aabb.h
    src/base/angle.cc src/base/angle.h
    src/base/axisangle.cc src/base/axisangle.h
    src/base/cint.cc src/base/cint.h
    src/base/colors.cc src/base/colors.h
    src/base/default_interpolate.cc src/base/default_interpolate.h
    src/base/editdistance.cc src/base/editdistance.h
    src/base/editdistance.search.cc src/base/editdistance.search.h
    src/base/functional.cc src/base/functional.h
    src/base/ints.cc src/base/ints.h
    src/base/lrud.cc src/base/lrud.h
    src/base/mat2.cc src/base/mat2.h
    src/base/mat3.cc src/base/mat3.h
    src/base/mat4.cc src/base/mat4.h
    src/base/memorychunk.cc src/base/memorychunk.h
    src/base/minmax.cc src/base/minmax.h
    src/base/numeric.cc src/base/numeric.h
    src/base/numparse.cc src/base/numparse.h
    src/base/os.cc src/base/os.h
    src/base/plane.cc src/base/plane.h
    src/base/polarcoord.cc src/base/polarcoord.h
    src/base/quat.cc src/base/quat.h
    src/base/random.cc src/base/random.h
    src/base/range.cc src/base/range.h
    src/base/ray.cc src/base/ray.h
    src/base/rect.cc src/base/rect.h
    src/base/result.cc src/base/result.h
    src/base/rgb.cc src/base/rgb.h
    src/base/size2.cc src/base/size2.h
    src/base/string_io.cc src/base/string_io.h
    src/base/stringbuilder.cc src/base/stringbuilder.h
    src/base/stringmerger.cc src/base/stringmerger.h
    src/base/stringutils.cc src/base/stringutils.h
    src/base/vec.base.cc src/base/vec.base.h
    src/base/vec2.cc src/base/vec2.h
    src/base/vec3.cc src/base/vec3.h
    src/base/vec4.cc src/base/vec4.h

    src/base/default_parse.cc src/base/default_parse.h
    src/base/enumtostring.cc src/base/enumtostring.h

    src/base/pch.private.h
    src/base/pch.public.h
)

source_group("" FILES ${app_src})

set(src
    ${app_src}
)

add_library(base STATIC ${src})
target_precompile_headers(base
    PUBLIC src/base/pch.public.h
    PRIVATE src/base/pch.private.h
)


add_library(euph::base ALIAS base)
set_target_properties(base PROPERTIES FOLDER "Libraries")
target_include_directories(base
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(base
    PUBLIC
        euph::log
        euph::assert
        euph::fmt
        external::range
        external::magic_enum
    PRIVATE
        euph::project_options
        euph::project_warnings
)

###############################################################################


###############################################################################
# test project

set(test_src
    src/base/angle.test.cc
    src/base/axisangle.test.cc
    src/base/editdistance.test.cc
    src/base/enumtostring.test.cc
    src/base/mat4.test.cc
    src/base/numeric.test.cc
    src/base/numparse.test.cc
    src/base/quat.test.cc
    src/base/range.test.cc
    src/base/rgb.test.cc
    src/base/stringbuilder.test.cc
    src/base/stringmerger.test.cc
    src/base/stringutils.test.cc
    src/base/vec2.test.cc
    src/base/vec3.test.cc
    src/base/vec4.test.cc
    src/base/rect.test.cc
)
# set(test_main_src test/main.cc)
source_group("" FILES ${test_src})
# source_group("main" FILES ${test_main_src})
add_executable(base_test ${test_src} ${test_main_src})
set_target_properties(base_test PROPERTIES FOLDER "Tests")
target_link_libraries(base_test PRIVATE euph::project_options euph::project_warnings euph::base euph::tests)
catch_discover_tests(base_test)
