set(app_src
    button.cc button.h
    container.cc container.h
    layout.cc layout.h
    layoutcontainer.cc layoutcontainer.h
    layoutdata.cc layoutdata.h
    load.cc load.h
    panelwidget.cc panelwidget.h
    root.cc root.h
    skin.cc skin.h
    textdata.cc textdata.h
    uistate.cc uistate.h
    visitor.cc visitor.h
    widget.cc widget.h
)
set(src ${app_src})
source_group("" FILES ${app_src})

add_library(gui STATIC ${src})
target_link_libraries(gui
    core
    render
    file_types
)
