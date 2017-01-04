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

add_library(render STATIC ${src})

target_link_libraries(render
    ${OPENGL_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    )
