###############################################################################
# tests project

set(src
    src/eu/tests/false_string.cc src/eu/tests/false_string.h
    src/eu/tests/vectorequals.h
    src/eu/tests/vectortostring.h
    src/eu/tests/equaler.cc src/eu/tests/equaler.h
    src/eu/tests/approx.cc src/eu/tests/approx.h
    src/eu/tests/approx_equal.cc src/eu/tests/approx_equal.h
)
source_group("" FILES ${src})

add_library(tests STATIC ${src})
add_library(euph::tests ALIAS tests)
set_target_properties(tests PROPERTIES FOLDER "Libraries")
target_include_directories(tests
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_precompile_headers(tests PRIVATE src/eu/tests/pch.private.h)
target_link_libraries(tests
    PUBLIC
        external::catch
        euph::base
    PRIVATE
        euph::project_options
        euph::project_warnings
)
