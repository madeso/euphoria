###############################################################################
# base project

set(app_src
    src/base/editdistance.cc src/base/editdistance.h
    src/base/editdistance.search.cc src/base/editdistance.search.h
    src/base/functional.cc src/base/functional.h
    src/base/stringmerger.cc src/base/stringmerger.h
    src/base/cint.cc src/base/cint.h
    src/base/ints.cc src/base/ints.h
    src/base/stringbuilder.cc src/base/stringbuilder.h
    src/base/os.cc src/base/os.h
    src/base/result.cc src/base/result.h
    src/base/stringutils.cc src/base/stringutils.h
    src/base/memorychunk.cc src/base/memorychunk.h

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
    PRIVATE
        euph::project_options
        euph::project_warnings
)

###############################################################################


###############################################################################
# test project

set(test_src
    src/base/editdistance.test.cc
    src/base/stringbuilder.test.cc
    src/base/stringmerger.test.cc
    src/base/stringutils.test.cc
)
# set(test_main_src test/main.cc)
source_group("" FILES ${test_src})
# source_group("main" FILES ${test_main_src})
add_executable(base_test ${test_src} ${test_main_src})
set_target_properties(base_test PROPERTIES FOLDER "Tests")
target_link_libraries(base_test PRIVATE euph::project_options euph::project_warnings euph::base euph::tests)
catch_discover_tests(base_test)
