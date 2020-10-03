set(app_src main.cc)
set(src ${app_src})
source_group("" FILES ${app_src})

add_executable(pixel WIN32 MACOSX_BUNDLE ${src})
target_link_libraries(pixel
    core
    render
    window
    imgui
    sdl2
)

