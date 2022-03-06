###############################################################################
# assert project

set(app_src
    src/assert/assert.cc src/assert/assert.h
)

source_group("" FILES ${app_src})

set(src
    ${app_src}
)

add_library(assert STATIC ${src})
add_library(euph::assert ALIAS assert)
set_target_properties(assert PROPERTIES FOLDER "Libraries")
target_include_directories(assert
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(assert
    PUBLIC
        euph::log
    PRIVATE
        euph::project_options
        euph::project_warnings
)
