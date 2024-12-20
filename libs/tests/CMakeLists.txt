###############################################################################
# tests project

set(src
    src/tests/approx.cc src/tests/approx.h
    src/tests/approx_equal.cc src/tests/approx_equal.h
)
source_group("" FILES ${src})

add_library(tests STATIC ${src})
add_library(euph::tests ALIAS tests)
set_target_properties(tests PROPERTIES FOLDER "Libraries")
target_include_directories(tests
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(tests
    PUBLIC
        external::catch
        external::catch_main
        euph::base
    PRIVATE
        euph::project_options
        euph::project_warnings
)
