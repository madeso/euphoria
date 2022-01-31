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
target_include_directories(assert
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(assert
    PUBLIC
        log
    PRIVATE
        project_options
        project_warnings
)
