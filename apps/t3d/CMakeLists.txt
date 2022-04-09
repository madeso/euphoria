###############################################################################
# t3d project

set(src
    src/t3d/editor.cc src/t3d/editor.h
    src/t3d/filelist.cc src/t3d/filelist.h
    src/t3d/grid.cc src/t3d/grid.h
    src/t3d/keyboardstate.cc src/t3d/keyboardstate.h
    src/t3d/main.cc
    src/t3d/t3d.cc src/t3d/t3d.h
    src/t3d/tilelibrary.cc src/t3d/tilelibrary.h
    src/t3d/tool.cc src/t3d/tool.h
    src/t3d/tool_notool.cc src/t3d/tool_notool.h
    src/t3d/tool_placemeshonplane.cc src/t3d/tool_placemeshonplane.h
    src/t3d/toolstack.cc src/t3d/toolstack.h
)
source_group("" FILES ${src})

add_executable(t3d WIN32 MACOSX_BUNDLE ${src})
set_win32_defines(t3d)
set_target_properties(t3d PROPERTIES FOLDER "Applications")
target_include_directories(t3d
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(t3d
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::core
        euph::render
        euph::window
        external::imgui
        external::sdl2
)

