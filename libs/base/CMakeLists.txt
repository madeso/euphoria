###############################################################################
# base project

set(app_src
    src/eu/base/numeric.cc src/eu/base/numeric.h
    src/eu/base/range.cc src/eu/base/range.h
    src/eu/base/angle.cc src/eu/base/angle.h
    src/eu/base/axisangle.cc src/eu/base/axisangle.h
    src/eu/base/colors.cc src/eu/base/colors.h
    src/eu/base/ints.cc src/eu/base/ints.h
    src/eu/base/mat4.cc src/eu/base/mat4.h
    src/eu/base/quat.cc src/eu/base/quat.h
    src/eu/base/vec2.cc src/eu/base/vec2.h
    src/eu/base/vec3.cc src/eu/base/vec3.h
    src/eu/base/vec4.cc src/eu/base/vec4.h
    src/eu/base/hash.string.cc src/eu/base/hash.string.h
    src/eu/base/guid.cc src/eu/base/guid.h
    src/eu/base/rect.cc src/eu/base/rect.h
    src/eu/base/size.cc src/eu/base/size.h
    src/eu/base/memorychunk.cc src/eu/base/memorychunk.h

    src/eu/base/feature_flags.h
    src/eu/base/cint.h
    src/eu/base/cpp.h

    src/eu/base/pch.private.h
    src/eu/base/pch.public.h
)

source_group("" FILES ${app_src})

set(src
    ${app_src}
)

add_library(base STATIC ${src})
target_precompile_headers(base PRIVATE src/eu/base/pch.private.h)


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
        external::guid
    PRIVATE
        euph::project_options
        euph::project_warnings
)

###############################################################################


###############################################################################
# test project

set(test_src
    src/eu/base/numeric.test.cc
    src/eu/base/angle.test.cc
    src/eu/base/range.test.cc
    src/eu/base/axisangle.test.cc
    src/eu/base/mat4.test.cc
    src/eu/base/quat.test.cc
    src/eu/base/vec2.test.cc
    src/eu/base/vec3.test.cc
    src/eu/base/vec4.test.cc
    src/eu/base/hash.string.test.cc
    src/eu/base/guid.test.cc
    src/eu/base/rect.test.cc
)
# set(test_main_src test/main.cc)
source_group("" FILES ${test_src})
# source_group("main" FILES ${test_main_src})
add_executable(base_test ${test_src} ${test_main_src})
target_precompile_headers(base_test PRIVATE src/eu/base/pch.private.h)
set_target_properties(base_test PROPERTIES FOLDER "Tests")
target_link_libraries(base_test
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::base
        euph::tests
)
catch_discover_tests(base_test)
