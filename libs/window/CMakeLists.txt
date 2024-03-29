###############################################################################
# window project

set(app_src
    src/window/app.cc src/window/app.h
    src/window/canvas.cc src/window/canvas.h
    src/window/engine.cc src/window/engine.h
    src/window/fakesdlevent.h src/window/fakesdlevent.cc
    src/window/filesystem.cc src/window/filesystem.h
    src/window/imgui_extra.cc src/window/imgui_extra.h
    src/window/imgui_icons.h src/window/imgui_icons.cc
    src/window/imguilibrary.cc src/window/imguilibrary.h
    src/window/imguizmo.cc src/window/imguizmo.h
    src/window/key.cc src/window/key.h
    src/window/log.cc src/window/log.h
    src/window/open_color.imgui.cc src/window/open_color.imgui.h
    src/window/sdlglcontext.cc src/window/sdlglcontext.h
    src/window/sdllibrary.cc src/window/sdllibrary.h
    src/window/sdlwindow.cc src/window/sdlwindow.h
    src/window/timer.cc src/window/timer.h
)
set(src ${app_src})
source_group("" FILES ${app_src})


# embed fonts
add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/font_material_icons.h"
  COMMAND  binary_to_compressed_c
  ARGS ${CMAKE_CURRENT_SOURCE_DIR}/data/MaterialIcons-Regular.ttf MaterialIcons > font_material_icons.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating binary font"
  )
set(src ${src} ${CMAKE_CURRENT_BINARY_DIR}/font_material_icons.h)
source_group("fonts" FILES ${CMAKE_CURRENT_BINARY_DIR}/font_material_icons.h)

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
add_library(euph::window ALIAS window)
set_target_properties(window PROPERTIES FOLDER "Libraries")
target_include_directories(window
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(window
    PUBLIC
        external::icon_font_cpp_headers
        external::open_color
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::core
        euph::render
        euph::fmt
        external::imgui
        external::icon_font_cpp_headers
        external::sdl2
        external::glad
        external::imguizmo
)
