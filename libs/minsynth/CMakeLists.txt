###############################################################################
# minsynth project

set(src
    src/minsynth/synth.cc src/minsynth/synth.h
)
source_group("" FILES ${src})

add_library(minsynth STATIC ${src})
set_target_properties(minsynth PROPERTIES FOLDER "Libraries")
target_include_directories(minsynth
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(minsynth
    PUBLIC
        core
    PRIVATE
        project_options
        project_warnings
)


###############################################################################
# test project

set(test_src
    src/minsynth/minsynth.test.cc
)
set(test_main_src test/main.cc)
source_group("" FILES ${test_src})
source_group("main" FILES ${test_main_src})
add_executable(minsynth_test ${test_src} ${test_main_src})
set_target_properties(minsynth_test PROPERTIES FOLDER "Tests")
target_link_libraries(minsynth_test
    PRIVATE
        project_options
        project_warnings
        minsynth
        tests
)

catch_discover_tests(minsynth_test)
