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

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
SET(Gaf_CUSTOM_PREFIX gaf_)
GAF_GENERATE_CPP(GUI_PROTO_SRCS GUI_PROTO_HDRS ../file_types/gui.gaf)
set(src ${src} ${pbjson_src} ${GUI_PROTO_SRCS} ${GUI_PROTO_HDRS})

add_library(gui STATIC ${src})

target_link_libraries(gui
                      core
                      render
                      )
