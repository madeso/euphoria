set(app_src
    NotoSansDisplay-Regular.ttf
    material_design_icons.ttf
    canvas.cc canvas.h
    engine.cc engine.h
    fakesdlevent.h
    filesystem.cc filesystem.h
    imgui_ext.cc imgui_ext.h
    imgui_extra.cc imgui_extra.h
    imgui_icons.h
    imguilibrary.cc imguilibrary.h
    key.cc key.h
    sdlglcontext.cc sdlglcontext.h
    sdllibrary.cc sdllibrary.h
    sdlwindow.cc sdlwindow.h
    timer.cc timer.h
    timgui.h
)
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
    ${IMGUI_DIR}/imstb_rectpack.h
    ${IMGUI_DIR}/imstb_textedit.h
    ${IMGUI_DIR}/imstb_truetype.h
    ${IMGUI_DIR}/examples/imgui_impl_sdl.cpp
    ${IMGUI_DIR}/examples/imgui_impl_sdl.h
    ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/examples/imgui_impl_opengl3.h
    # also include fake imgui for completness
    fake_imgui/imgui.h
    )
set(src ${src} ${src_imgui})
source_group("imgui" FILES ${src_imgui})
source_group("imgui/fake_imgui" FILES fake_imgui/imgui.h)

# fonts
add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/font_material_design_icons.h"
  COMMAND  binary_to_compressed_c
  ARGS ${CMAKE_CURRENT_SOURCE_DIR}/material_design_icons.ttf MaterialDesignIcons > font_material_design_icons.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating binary font"
  )
set(src ${src} ${CMAKE_CURRENT_BINARY_DIR}/font_material_design_icons.h)
source_group("fonts" FILES ${CMAKE_CURRENT_BINARY_DIR}/font_material_design_icons.h)

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
