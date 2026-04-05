###############################################################################
# core project

set(core_src
    src/core/image.cc src/core/image.h
    src/core/loadedfont.cc src/core/loadedfont.h
    src/core/tablelayout.cc src/core/tablelayout.h
    src/core/utf8.cc src/core/utf8.h
    src/core/scurve.cc src/core/scurve.h
    src/core/ui.cc src/core/ui.h

    src/core/hash.h

    src/core/pch.private.h
)

source_group("" FILES ${core_src})

set(src
    ${core_src}
)

add_library(core STATIC ${src})
target_precompile_headers(core PRIVATE src/core/pch.private.h)

add_library(euph::core ALIAS core)
set_target_properties(core PROPERTIES FOLDER "Libraries")
target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(core
    PUBLIC
        euph::assert
        euph::base
        external::stb_truetype
        embed::embed
    PRIVATE
        euph::project_options
        euph::project_warnings
)

###############################################################################
# test project

set(test_src
    src/core/tablelayout.test.cc
    src/core/scurve.test.cc
    src/core/ui.test.cc
)
source_group("" FILES ${test_src})
add_executable(core_test ${test_src} ${test_main_src})
target_precompile_headers(core_test PRIVATE src/core/pch.private.h)
set_target_properties(core_test PROPERTIES FOLDER "Tests")
target_link_libraries(core_test
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::core
        euph::tests
)
catch_discover_tests(core_test)
