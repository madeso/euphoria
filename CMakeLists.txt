cmake_minimum_required(VERSION 3.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(music)

ADD_SUBDIRECTORY(external/rtmidi)
set(BUILD_STATIC_LIBS ON)
set(RTMIDI_BUILD_STATIC_LIBS "")
set(RTMIDI_BUILD_SHARED_LIBS "")
include_directories(SYSTEM external/rtmidi)

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
  add_compile_options(-Wno-error=deprecated)
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
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

add_executable(binary_to_compressed_c external/imgui/misc/fonts/binary_to_compressed_c.cpp)

add_subdirectory(musikmaskin)

