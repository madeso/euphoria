###############################################################################
# undef_windows project

set(app_src
    undef_windows/undef_windows.cc undef_windows/undef_windows.h
)

source_group("" FILES ${app_src})

set(src
    ${app_src}
)

add_library(undef_windows STATIC ${src})
target_include_directories(undef_windows
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
add_library(undef_windows::undef_windows ALIAS undef_windows)
