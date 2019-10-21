file(GLOB app_src *.cc;*.h;*.inl)
file(GLOB app_only_src *.cc)
set(src ${app_src})
source_group("" FILES ${app_src})

# glad
include_directories(SYSTEM ../external/glad/include)
#set(src_glad ../external/glad/src/glad.c ../external/glad/include/glad/glad.h)
#set(src ${src} ${src_glad})
#source_group("glad" FILES ${src_glad})

# imgui
include_directories(SYSTEM ../external/imgui/imgui/examples)
include_directories(SYSTEM fake_imgui)
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
# imgui
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui/imgui/")
set(src_imgui
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/imconfig.h
    # todo: look into how different stb libs work together
    ${IMGUI_DIR}/stb_rect_pack.h
    ${IMGUI_DIR}/stb_textedit.h
    ${IMGUI_DIR}/stb_truetype.h
    ${IMGUI_DIR}/examples/imgui_impl_sdl.cpp
    ${IMGUI_DIR}/examples/imgui_impl_sdl.h
    ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/examples/imgui_impl_opengl3.h
    )
set(src ${src} ${src_imgui})
source_group("imgui" FILES ${src_imgui})

# fonts
add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/font_forkawesome.h"
  COMMAND  binary_to_compressed_c
  ARGS ${CMAKE_CURRENT_SOURCE_DIR}/forkawesome.ttf ForkAwesome > font_forkawesome.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating binary font"
  )
set(src ${src} ${CMAKE_CURRENT_BINARY_DIR}/font_forkawesome.h)
source_group("fonts" FILES ${CMAKE_CURRENT_BINARY_DIR}/font_forkawesome.h)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/font_noto_sans_display.h"
  COMMAND  binary_to_compressed_c
  ARGS ${CMAKE_CURRENT_SOURCE_DIR}/NotoSansDisplay-Regular.ttf NotoSansDisplay > font_noto_sans_display.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating binary font: Noto Sans Display"
)
set(src ${src} ${CMAKE_CURRENT_BINARY_DIR}/font_noto_sans_display.h)
source_group("fonts" FILES ${CMAKE_CURRENT_BINARY_DIR}/font_noto_sans_display.h)

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

include_directories(SYSTEM ../external/IconFontCppHeaders/)

include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

add_library(window STATIC ${src})

target_link_libraries(window
                      render
                      core
                      ${SDL2_LIBRARY}
                      )
