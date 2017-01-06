file(GLOB app_src_glob *.cpp;*.cc;*.h;*.inl)

find_package(SDL2 REQUIRED)
include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(SPRITE_PROTO_SRCS SPRITE_PROTO_HDRS ../proto/scalingsprite.proto)

# include_directories(SYSTEM ../external/glm/include)

set(app_src ${app_src_glob} ${pbjson_src} ${SPRITE_PROTO_SRCS} ${SPRITE_PROTO_HDRS})
source_group("" FILES ${app_src})

set(all_src ${app_src} ${src_shaders} ${src_glew} ${src_aheasing})

add_executable(spacetyper WIN32 MACOSX_BUNDLE ${all_src})
set_target_properties(spacetyper
                      PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/dist"
                      DEBUG_OUTPUT_NAME "spacetyper-debug"
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/dist"
                      RELEASE_OUTPUT_NAME "spacetyper"
                      )

target_link_libraries(spacetyper
                      core
                      gui
                      render
                      ${OPENGL_LIBRARIES}
                      ${SDL2_LIBRARY}
                      )

if(CMAKE_COMPILER_IS_GNUCC)
  set_property(TARGET spacetyper APPEND_STRING PROPERTY COMPILE_FLAGS -Wall)
endif()

