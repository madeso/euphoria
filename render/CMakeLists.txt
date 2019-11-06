file(GLOB app_src *.cc;*.h;*.inl)
file(GLOB app_only_src *.cc)
set(src ${app_src})
source_group("" FILES ${app_src})

include_directories(SYSTEM ../external/stb)

# glad
include_directories(SYSTEM ../external/glad/include)
set(src_glad ../external/glad/src/glad.c ../external/glad/include/glad/glad.h)
set(src ${src} ${src_glad})
source_group("glad" FILES ${src_glad})

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
SET(Gaf_CUSTOM_PREFIX gaf_)
GAF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
  ../file_types/scalingsprite.gaf
  ../file_types/materialshader.gaf
  ../file_types/font.gaf
  )
set(src ${src} ${pbjson_src} ${PROTO_SRCS} ${PROTO_HDRS})

add_library(render STATIC ${src})

target_link_libraries(render
                      core
                      ${OPENGL_LIBRARIES}
                      ${CMAKE_DL_LIBS}
                      )
