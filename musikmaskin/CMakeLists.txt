file(GLOB app_src_glob *.cpp;*.cc;*.h;*.inl)

set(app_src ${app_src_glob})
source_group("" FILES ${app_src})

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/font_noto_sans_display.h"
  COMMAND  binary_to_compressed_c
  ARGS ${CMAKE_CURRENT_SOURCE_DIR}/NotoSansDisplay-Regular.ttf NotoSansDisplay > font_noto_sans_display.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating binary font: Noto Sans Display"
)
set(app_src ${app_src} ${CMAKE_CURRENT_BINARY_DIR}/font_noto_sans_display.h)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

set(app_src ${app_src}
    # standard imgui
    ../external/imgui/imgui.cpp
    ../external/imgui/imgui.h
    ../external/imgui/imgui_demo.cpp
    ../external/imgui/imgui_draw.cpp
    ../external/imgui/imgui_internal.h
    ../external/imgui/stb_rect_pack.h
    ../external/imgui/stb_textedit.h
    ../external/imgui/stb_truetype.h
    # sdl binding
    ../external/imgui/examples/imgui_impl_sdl.h
    ../external/imgui/examples/imgui_impl_sdl.cpp
    # opengl binding
    ../external/imgui/examples/imgui_impl_opengl3.cpp
    ../external/imgui/examples/imgui_impl_opengl3.h
    # opengl loader
    ../external/glad/src/glad.c
    ../external/glad/include/glad/glad.h
    )

add_executable(musikmaskin WIN32 MACOSX_BUNDLE ${app_src})
target_link_libraries(musikmaskin
                      ${SDL2_LIBRARY}
                      ${CMAKE_DL_LIBS}
                      rtmidi
                      )

