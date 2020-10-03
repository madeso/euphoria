set(app_src
    editor.cc editor.h
    filelist.cc filelist.h
    grid.cc grid.h
    main.cc
    t3d.cc t3d.h
    tilelibrary.cc tilelibrary.h
    tool.cc tool.h
    tool_notool.cc tool_notool.h
    tool_placemeshonplane.cc tool_placemeshonplane.h
    toolstack.cc toolstack.h
)
set(src ${app_src})
source_group("" FILES ${app_src})

add_executable(t3d WIN32 MACOSX_BUNDLE ${src})
target_link_libraries(t3d
    core
    render
    window
    sdl2
)
