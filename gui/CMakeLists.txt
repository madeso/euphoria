file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(GUI_PROTO_SRCS GUI_PROTO_HDRS ../proto/gui.proto)
set(src ${src} ${pbjson_src} ${GUI_PROTO_SRCS} ${GUI_PROTO_HDRS})

add_library(gui STATIC ${src})

target_link_libraries(gui
                      core
                      render
                      ${PROTOBUF_LIBRARIES}
                      )