set(app_src main.cc)
set(src ${app_src})
source_group("" FILES ${app_src})

add_executable(painter WIN32 MACOSX_BUNDLE ${src})
target_link_libraries(painter
    core
    render
    window
    imgui
    sdl2
)

