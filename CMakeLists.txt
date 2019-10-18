cmake_minimum_required(VERSION 3.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(euphoria)

option(BUILD_MIDI_SUPPORT "include midi support" ON)
option(BUILD_ENTITY_DEBUG_COMPONENT "when converting components, (debug) assert of bad casts" OFF)
set(BUILD_TYPEINFO_IMPL "rtti" CACHE STRING "rtti, ctti, custom_hashed, custom_rt")

set(OpenGL_GL_PREFERENCE "GLVND")

set(BUILD_TYPEINFO_IMPL_RTTI FALSE)
set(BUILD_TYPEINFO_IMPL_CTTI FALSE)
set(BUILD_TYPEINFO_IMPL_CUSTOM_HASHED FALSE)
set(BUILD_TYPEINFO_IMPL_CUSTOM_RT FALSE)

message(STATUS "BUILD_TYPEINFO_IMPL: ${BUILD_TYPEINFO_IMPL}")
if(BUILD_TYPEINFO_IMPL STREQUAL "rtti")
  set(BUILD_TYPEINFO_IMPL_RTTI TRUE)
elseif(BUILD_TYPEINFO_IMPL STREQUAL "ctti")
  set(BUILD_TYPEINFO_IMPL_CTTI TRUE)
elseif(BUILD_TYPEINFO_IMPL STREQUAL "custom_hashed")
  set(BUILD_TYPEINFO_IMPL_CUSTOM_HASHED TRUE)
elseif(BUILD_TYPEINFO_IMPL STREQUAL "custom_rt")
  set(BUILD_TYPEINFO_IMPL_CUSTOM_RT TRUE)
else()
  message(STATUS "Invalid build status: ${BUILD_TYPEINFO_IMPL}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Gaf_CUSTOM_ARGUMENTS_FROM_FILE ${CMAKE_CURRENT_SOURCE_DIR}/gaf_options.txt)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

if(${BUILD_MIDI_SUPPORT})
  add_subdirectory(external/rtmidi)
  set(BUILD_STATIC_LIBS ON)
  set(RTMIDI_BUILD_STATIC_LIBS "")
  set(RTMIDI_BUILD_SHARED_LIBS "")
  include_directories(SYSTEM external/rtmidi)
endif(${BUILD_MIDI_SUPPORT})

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
# message(STATUS "Assimp includes at ${assimp_INCLUDE_DIRS}")
# message(STATUS "Assimp libraries at ${assimp_LIBRARIES}")
find_package(SDL2 REQUIRED)
# message(STATUS "sdl2 include directory ${SDL2_INCLUDE_DIR}")

include(${CMAKE_CURRENT_SOURCE_DIR}/external/gaf/gaf.cmake)

include_directories(SYSTEM
                    ${CMAKE_CURRENT_SOURCE_DIR}/external/rapidjson/include
                    )

set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/osx-bundle.plist")

# compiler flags
if(${MSVC})
  add_definitions(/D _CRT_SECURE_NO_WARNINGS)

  # character set to Not Set=ASCII/SBCS
  # http://public.kitware.com/Bug/view.php?id=12189
  add_definitions(/D _SBCS)
endif()

# disable rtti

if(NOT BUILD_TYPEINFO_IMPL_RTTI)
  if(MSVC)
    add_compile_options(/GR-)
  else()
    add_compile_options(-fno-rtti)
  endif()
endif()

# enable all warnings
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
  add_compile_options(-Wshadow)
  add_compile_options(-Wnon-virtual-dtor)
  # add_compile_options(-Wold-style-cast) # todo...
  add_compile_options(-Wcast-align)
  add_compile_options(-Wunused)
  add_compile_options(-Woverloaded-virtual)
  #add_compile_options(-Wconversion)
  #add_compile_options(-Wsign-conversion)
  add_compile_options(-Wformat=2)
  # add_compile_options(-Wlifetime)

  add_compile_options(-Wno-error=deprecated)
endif()


# force visual studio and the like to only have debug and release builds
if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES Debug Release)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset the configurations to what we need" FORCE)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

if(BUILD_TYPEINFO_IMPL_CTTI)
  include_directories(SYSTEM external/ctti/include)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_in.h ${CMAKE_CURRENT_BINARY_DIR}/euph_generated_config.h)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(core)

add_definitions(-DIMGUI_USER_CONFIG="imgui_config.h")
add_executable(binary_to_compressed_c external/imgui/imgui/misc/fonts/binary_to_compressed_c.cpp)

add_subdirectory(tracery)
add_subdirectory(minsynth)
add_subdirectory(base64)
add_subdirectory(render)
add_subdirectory(window)
add_subdirectory(gui)
add_subdirectory(duk)
add_subdirectory(tests)
add_subdirectory(markov)
add_subdirectory(demo)
add_subdirectory(engine)
add_subdirectory(chatbot_test)
add_subdirectory(worldgen)
add_subdirectory(img)
add_subdirectory(editor)
add_subdirectory(painter)
add_subdirectory(tabletool)
add_subdirectory(musikmaskin)
