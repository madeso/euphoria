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
include_directories(SYSTEM ../external/imgui-1.50/)
set(src_imgui
    ../external/imgui-1.50/examples/sdl_opengl3_example/imgui_impl_sdl_gl3.cpp
    ../external/imgui-1.50/examples/sdl_opengl3_example/imgui_impl_sdl_gl3.h
    )
set(src ${src} ${src_imgui})
source_group("imgui" FILES ${src_imgui})

include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

add_library(window STATIC ${src})

target_link_libraries(window
                      render
                      core
                      ${SDL2_LIBRARY}
                      )

if(CLANG_TIDY_EXE)
  # -fix
  ADD_CUSTOM_TARGET(tidy-window
                    COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} ${app_only_src}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if(CLANG_FORMAT_EXE)
  # -fix
  ADD_CUSTOM_TARGET(format-window
                    COMMAND ${CLANG_FORMAT_EXE} -i ${app_src}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
