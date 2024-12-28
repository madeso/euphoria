###############################################################################
# base project

set(app_src
    src/base/numeric.cc src/base/numeric.h
    src/base/range.cc src/base/range.h
    src/base/angle.cc src/base/angle.h
    src/base/axisangle.cc src/base/axisangle.h
    src/base/colors.cc src/base/colors.h
    src/base/ints.cc src/base/ints.h
    src/base/mat4.cc src/base/mat4.h
    src/base/quat.cc src/base/quat.h
    src/base/vec2.cc src/base/vec2.h
    src/base/vec3.cc src/base/vec3.h
    src/base/vec4.cc src/base/vec4.h
    src/base/hash.string.cc src/base/hash.string.h

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
    src/base/numeric.test.cc
    src/base/angle.test.cc
    src/base/range.test.cc
    src/base/axisangle.test.cc
    src/base/mat4.test.cc
    src/base/quat.test.cc
    src/base/vec2.test.cc
    src/base/vec3.test.cc
    src/base/vec4.test.cc
    src/base/hash.string.test.cc
)
# set(test_main_src test/main.cc)
source_group("" FILES ${test_src})
# source_group("main" FILES ${test_main_src})
add_executable(base_test ${test_src} ${test_main_src})
set_target_properties(base_test PROPERTIES FOLDER "Tests")
target_link_libraries(base_test
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::base
        euph::tests
)
catch_discover_tests(base_test)
