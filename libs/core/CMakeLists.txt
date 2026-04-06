###############################################################################
# core project

set(core_src
    src/eu/core/image.cc src/eu/core/image.h
    src/eu/core/loadedfont.cc src/eu/core/loadedfont.h
    src/eu/core/tablelayout.cc src/eu/core/tablelayout.h
    src/eu/core/utf8.cc src/eu/core/utf8.h
    src/eu/core/scurve.cc src/eu/core/scurve.h
    src/eu/core/ui.cc src/eu/core/ui.h

    src/eu/core/hash.h

    src/eu/core/pch.private.h
)

source_group("" FILES ${core_src})

set(src
    ${core_src}
)

add_library(core STATIC ${src})
target_precompile_headers(core PRIVATE src/eu/core/pch.private.h)

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
    src/eu/core/tablelayout.test.cc
    src/eu/core/scurve.test.cc
    src/eu/core/ui.test.cc
)
source_group("" FILES ${test_src})
add_executable(core_test ${test_src} ${test_main_src})
target_precompile_headers(core_test PRIVATE src/eu/core/pch.private.h)
set_target_properties(core_test PROPERTIES FOLDER "Tests")
target_link_libraries(core_test
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::core
        euph::tests
)
catch_discover_tests(core_test)
