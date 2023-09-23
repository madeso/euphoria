###############################################################################
# io project

set(app_src
    src/io/json.cc src/io/json.h

    src/io/io.cc src/io/io.h

    src/io/vfs.cc src/io/vfs.h
    src/io/vfs_path.cc src/io/vfs_path.h
)

source_group("" FILES ${app_src})

set(src
    ${app_src}
)

add_library(io STATIC ${src})
add_library(euph::io ALIAS io)
set_target_properties(io PROPERTIES FOLDER "Libraries")
target_include_directories(io
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(io
    PUBLIC
        euph::log
        euph::assert
        euph::fmt
        euph::base
        jsonh::jsonh
    PRIVATE
        euph::project_options
        euph::project_warnings
)



###############################################################################


###############################################################################
# test project

set(test_src
    src/io/vfs.test.cc
    src/io/vfs_path.test.cc
)
# set(test_main_src test/main.cc)
source_group("" FILES ${test_src})
# source_group("main" FILES ${test_main_src})
add_executable(io_test ${test_src} ${test_main_src})
set_target_properties(io_test PROPERTIES FOLDER "Tests")
target_link_libraries(io_test PRIVATE euph::project_options euph::project_warnings euph::io euph::tests)
catch_discover_tests(io_test)
