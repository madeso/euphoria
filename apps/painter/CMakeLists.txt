###############################################################################
# painter project

set(src
    src/painter/main.cc
)
source_group("" FILES ${src})

add_executable(painter WIN32 MACOSX_BUNDLE ${src})
set_target_properties(painter PROPERTIES FOLDER "Applications")
target_include_directories(painter
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(painter
    PRIVATE
        project_options
        project_warnings
        core
        render
        window
        imgui
        sdl2
)

