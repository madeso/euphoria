###############################################################################
# log project

set(app_src
    src/log/log.cc src/log/log.h
)

source_group("" FILES ${app_src})

set(src
    ${app_src}
)

add_library(log STATIC ${src})
add_library(euph::log ALIAS log)
set_target_properties(log PROPERTIES FOLDER "Libraries")
target_include_directories(log
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(log
    PUBLIC
        external::spdlog
        undef_windows::undef_windows
    PRIVATE
        euph::project_options
        euph::project_warnings
)
