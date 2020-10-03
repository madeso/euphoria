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


add_library(window STATIC ${src})

target_link_libraries(window
    PRIVATE
    core
    render
    imgui
    icon_font_cpp_headers
    sdl2
    glad
)
