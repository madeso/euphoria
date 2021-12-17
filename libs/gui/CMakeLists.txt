###############################################################################
# gui project

set(src
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
source_group("" FILES ${src})

add_library(gui STATIC ${src})
set_target_properties(gui PROPERTIES FOLDER "Libraries")
target_include_directories(gui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(gui
    PRIVATE
        project_options
        project_warnings
        core
        render
        file_types
)


###############################################################################
# example project

file(GLOB all_examples_src examples/*.cc)
source_group("" FILES ${all_examples_src})

add_executable(examples-gui MACOSX_BUNDLE examples/gui.cc)
set_target_properties(examples-gui PROPERTIES
    FOLDER "Examples/GUI"
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/dist/gui
)
target_link_libraries(examples-gui
    PRIVATE
        project_options
        project_warnings
        core
        gui
        render
        window
        imgui
        sdl2
)

