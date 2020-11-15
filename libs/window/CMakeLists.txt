set(app_src
    data/NotoSansDisplay-Regular.ttf
    data/material_design_icons.ttf
    src/window/canvas.cc src/window/canvas.h
    src/window/engine.cc src/window/engine.h
    src/window/fakesdlevent.h
    src/window/filesystem.cc src/window/filesystem.h
    src/window/imgui_ext.cc src/window/imgui_ext.h
    src/window/imgui_extra.cc src/window/imgui_extra.h
    src/window/imgui_icons.h
    src/window/imguilibrary.cc src/window/imguilibrary.h
    src/window/key.cc src/window/key.h
    src/window/sdlglcontext.cc src/window/sdlglcontext.h
    src/window/sdllibrary.cc src/window/sdllibrary.h
    src/window/sdlwindow.cc src/window/sdlwindow.h
    src/window/timer.cc src/window/timer.h
    src/window/timgui.h
)
set(src ${app_src})
source_group("" FILES ${app_src})


# fonts
add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/font_material_design_icons.h"
  COMMAND  binary_to_compressed_c
  ARGS ${CMAKE_CURRENT_SOURCE_DIR}/data/material_design_icons.ttf MaterialDesignIcons > font_material_design_icons.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating binary font"
  )
set(src ${src} ${CMAKE_CURRENT_BINARY_DIR}/font_material_design_icons.h)
source_group("fonts" FILES ${CMAKE_CURRENT_BINARY_DIR}/font_material_design_icons.h)

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/font_noto_sans_display.h"
  COMMAND  binary_to_compressed_c
  ARGS ${CMAKE_CURRENT_SOURCE_DIR}/data/NotoSansDisplay-Regular.ttf NotoSansDisplay > font_noto_sans_display.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating binary font: Noto Sans Display"
)
set(src ${src} ${CMAKE_CURRENT_BINARY_DIR}/font_noto_sans_display.h)
source_group("fonts" FILES ${CMAKE_CURRENT_BINARY_DIR}/font_noto_sans_display.h)

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})


add_library(window STATIC ${src})
target_include_directories(window
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(window
    PUBLIC
    icon_font_cpp_headers
    PRIVATE
    core
    render
    imgui
    icon_font_cpp_headers
    sdl2
    glad
)

