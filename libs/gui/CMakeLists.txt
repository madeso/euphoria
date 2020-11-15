set(app_src
    src/gui/button.cc src/gui/button.h
    src/gui/container.cc src/gui/container.h
    src/gui/layout.cc src/gui/layout.h
    src/gui/layoutcontainer.cc src/gui/layoutcontainer.h
    src/gui/layoutdata.cc src/gui/layoutdata.h
    src/gui/load.cc src/gui/load.h
    src/gui/panelwidget.cc src/gui/panelwidget.h
    src/gui/root.cc src/gui/root.h
    src/gui/skin.cc src/gui/skin.h
    src/gui/textdata.cc src/gui/textdata.h
    src/gui/uistate.cc src/gui/uistate.h
    src/gui/visitor.cc src/gui/visitor.h
    src/gui/widget.cc src/gui/widget.h
)
set(src ${app_src})
source_group("" FILES ${app_src})

add_library(gui STATIC ${src})
target_include_directories(gui
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(gui
    core
    render
    file_types
)

file(GLOB all_examples_src examples/*.cc)
source_group("" FILES ${all_examples_src})

add_executable(examples-gui WIN32 MACOSX_BUNDLE examples/gui.cc)
target_link_libraries(examples-gui core gui render window imgui sdl2)

