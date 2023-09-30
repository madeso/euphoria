###############################################################################
# tests project

set(src
    src/tests/approx.cc src/tests/approx.h
    src/tests/approx_equal.cc src/tests/approx_equal.h
    src/tests/stringeq.cc src/tests/stringeq.h
    src/tests/falsestring.cc src/tests/falsestring.h
    src/tests/utils.cc src/tests/utils.h
    src/tests/vectortostring.cc src/tests/vectortostring.h
    src/tests/vectorequals.cc src/tests/vectorequals.h
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
        euph::core
        euph::base
    PRIVATE
        euph::project_options
        euph::project_warnings
)


###############################################################################
# test project

set(test_src
    src/tests/stringeq.test.cc
)
set(test_main_src test/main.cc)
source_group("" FILES ${test_src})
source_group("main" FILES ${test_main_src})
add_executable(tests_test ${test_src} ${test_main_src})
set_target_properties(tests_test PROPERTIES FOLDER "Tests")
target_link_libraries(tests_test
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::tests
        euph::core
)

catch_discover_tests(tests_test)
