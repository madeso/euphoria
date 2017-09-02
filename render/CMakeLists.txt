file(GLOB app_src *.cc;*.h;*.inl)
file(GLOB app_only_src *.cc)
set(src ${app_src})
source_group("" FILES ${app_src})

include_directories(SYSTEM ../external/stb)

# freetype
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

# glad
include_directories(SYSTEM ../external/glad/include)
set(src_glad ../external/glad/src/glad.c ../external/glad/include/glad/glad.h)
set(src ${src} ${src_glad})
source_group("glad" FILES ${src_glad})

# imgui
include_directories(SYSTEM ../external/imgui-1.50/)
set(src_imgui
    ../external/imgui/imgui/imgui.cpp
    ../external/imgui/imgui/imgui.h
    ../external/imgui/imgui/imgui_demo.cpp
    ../external/imgui/imgui/imgui_draw.cpp
    ../external/imgui/imgui/imgui_internal.h
    ../external/imgui/imgui/imconfig.h
    # todo: look into how different stb libs work together
    ../external/imgui/imgui/stb_rect_pack.h
    ../external/imgui/imgui/stb_textedit.h
    ../external/imgui/imgui/stb_truetype.h
    )
set(src ${src} ${src_imgui})
source_group("imgui" FILES ${src_imgui})

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
  ../proto/scalingsprite.proto
  ../proto/materialshader.proto
  )
set(src ${src} ${pbjson_src} ${PROTO_SRCS} ${PROTO_HDRS})

add_library(render STATIC ${src})

target_link_libraries(render
                      core
                      ${OPENGL_LIBRARIES}
                      ${PROTOBUF_LIBRARIES}
                      ${FREETYPE_LIBRARIES}
                      ${CMAKE_DL_LIBS}
                      )

if(CLANG_TIDY_EXE)
  # -fix
  ADD_CUSTOM_TARGET(tidy-render
                    COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} ${app_only_src}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if(CLANG_FORMAT_EXE)
  # -fix
  ADD_CUSTOM_TARGET(format-render
                    COMMAND ${CLANG_FORMAT_EXE} -i ${app_src}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
