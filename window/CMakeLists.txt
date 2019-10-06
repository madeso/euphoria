file(GLOB app_src *.cc;*.h;*.inl)
file(GLOB app_only_src *.cc)
set(src ${app_src})
source_group("" FILES ${app_src})

# glad
include_directories(SYSTEM ../external/glad/include)
#set(src_glad ../external/glad/src/glad.c ../external/glad/include/glad/glad.h)
#set(src ${src} ${src_glad})
#source_group("glad" FILES ${src_glad})

# imgui
include_directories(SYSTEM
                    ../external/imgui/
                    )
set(src_imgui
    ../external/imgui/imgui_impl_sdl_gl3.cpp
    ../external/imgui/imgui_impl_sdl_gl3.h
    )
set(src ${src} ${src_imgui})
source_group("imgui" FILES ${src_imgui})

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/font_forkawesome.h"
  COMMAND  binary_to_compressed_c
  ARGS ${CMAKE_CURRENT_SOURCE_DIR}/forkawesome.ttf ForkAwesome > font_forkawesome.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating binary font"
  )
set(src ${src} ${CMAKE_CURRENT_BINARY_DIR}/font_forkawesome.h)
source_group("fonts" FILES ${CMAKE_CURRENT_BINARY_DIR}/font_forkawesome.h)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

include_directories(SYSTEM ../external/IconFontCppHeaders/)

include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

add_library(window STATIC ${src})

target_link_libraries(window
                      render
                      core
                      ${SDL2_LIBRARY}
                      )
