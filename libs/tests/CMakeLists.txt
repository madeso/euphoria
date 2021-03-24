###############################################################################
# tests project

set(src
    src/tests/approx.cc src/tests/approx.h
    src/tests/approx_equal.cc src/tests/approx_equal.h
    src/tests/stringeq.cc src/tests/stringeq.h
    src/tests/falsestring.cc src/tests/falsestring.h
    src/tests/utils.h
    src/tests/vectortostring.h
)
source_group("" FILES ${src})

add_library(tests STATIC ${src})
target_include_directories(tests
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(tests
    core
    catch
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
target_link_libraries(tests_test PUBLIC tests)
