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
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui/")
include_directories(SYSTEM ${IMGUI_DIR})
set(src_imgui
    ${IMGUI_DIR}/imgui/imgui.cpp
    ${IMGUI_DIR}/imgui/imgui.h
    ${IMGUI_DIR}/imgui/imgui_demo.cpp
    ${IMGUI_DIR}/imgui/imgui_draw.cpp
    ${IMGUI_DIR}/imgui/imgui_internal.h
    ${IMGUI_DIR}/imgui/imconfig.h
    # todo: look into how different stb libs work together
    ${IMGUI_DIR}/imgui/stb_rect_pack.h
    ${IMGUI_DIR}/imgui/stb_textedit.h
    ${IMGUI_DIR}/imgui/stb_truetype.h
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
