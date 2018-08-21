cmake_minimum_required(VERSION 3.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(music)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

find_package(OpenGL REQUIRED)

# enable all warnings
if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(SDL2 REQUIRED)

file(GLOB app_src_glob *.cpp;*.cc;*.h;*.inl)

include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

set(app_src ${app_src_glob})
source_group("" FILES ${app_src})

include_directories(SYSTEM external/imgui)
include_directories(SYSTEM external/imgui/examples)
include_directories(SYSTEM external/glad/include)
include_directories(SYSTEM fake/)

set(app_src ${app_src}
    # standard imgui
    external/imgui/imgui.cpp
    external/imgui/imgui.h
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_internal.h
    external/imgui/stb_rect_pack.h
    external/imgui/stb_textedit.h
    external/imgui/stb_truetype.h
    # sdl binding
    external/imgui/examples/imgui_impl_sdl.h
    external/imgui/examples/imgui_impl_sdl.cpp
    # opengl binding
    external/imgui/examples/imgui_impl_opengl3.cpp
    external/imgui/examples/imgui_impl_opengl3.h
    # opengl loader
    external/glad/src/glad.c
    external/glad/include/glad/glad.h
    )

add_executable(demo WIN32 MACOSX_BUNDLE ${app_src})
target_link_libraries(demo
                      ${SDL2_LIBRARY}
                      ${CMAKE_DL_LIBS}
                      )

