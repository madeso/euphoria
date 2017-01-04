file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})

include_directories(SYSTEM ../external/stb)

# freetype
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

include_directories(SYSTEM ../external/glew/include)
set(src_glew ../external/glew/glew.c ../external/glew/include/GL/glew.h)
set(src ${src} ${src_glew})
source_group("glew" FILES ${src_glew})

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(SPRITE_PROTO_SRCS SPRITE_PROTO_HDRS ../proto/scalingsprite.proto)
set(src ${src} ${pbjson_src} ${SPRITE_PROTO_SRCS} ${SPRITE_PROTO_HDRS})

add_library(render STATIC ${src})

target_link_libraries(render
                      libcore
    ${OPENGL_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    )
