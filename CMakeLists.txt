cmake_minimum_required(VERSION 3.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(euphoria)

OPTION(BUILD_USE_CLANG_FORMAT "try to use clang-format as a build command." OFF)
OPTION(BUILD_USE_CLANG_TIDY "try to use clang-tidy as a build command." OFF)
OPTION(BUILD_ENTITY_DEBUG_COMPONENT "when converting components, (debug) assert of bad casts" OFF)
SET(BUILD_TYPEINFO_IMPL "rtti" CACHE STRING "rtti, ctti, custom_hashed, custom_rt")

SET(BUILD_TYPEINFO_IMPL_RTTI FALSE)
SET(BUILD_TYPEINFO_IMPL_CTTI FALSE)
SET(BUILD_TYPEINFO_IMPL_CUSTOM_HASHED FALSE)
SET(BUILD_TYPEINFO_IMPL_CUSTOM_RT FALSE)

message(STATUS "BUILD_TYPEINFO_IMPL: ${BUILD_TYPEINFO_IMPL}")
if(BUILD_TYPEINFO_IMPL STREQUAL "rtti")
  SET(BUILD_TYPEINFO_IMPL_RTTI TRUE)
elseif(BUILD_TYPEINFO_IMPL STREQUAL "ctti")
  SET(BUILD_TYPEINFO_IMPL_CTTI TRUE)
elseif(BUILD_TYPEINFO_IMPL STREQUAL "custom_hashed")
  SET(BUILD_TYPEINFO_IMPL_CUSTOM_HASHED TRUE)
elseif(BUILD_TYPEINFO_IMPL STREQUAL "custom_rt")
  SET(BUILD_TYPEINFO_IMPL_CUSTOM_RT TRUE)
else()
  message(STATUS "Invalid build status: ${BUILD_TYPEINFO_IMPL}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(Gaf_CUSTOM_ARGUMENTS_FROM_FILE ${CMAKE_CURRENT_SOURCE_DIR}/gaf_options.txt)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

find_package(OpenGL REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/external/gaf/gaf.cmake)

include_directories(SYSTEM
                    ${CMAKE_CURRENT_SOURCE_DIR}/external/gsl-lite/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/external/rapidjson/include
                    )

SET(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/osx-bundle.plist")

# compiler flags
if(${MSVC})
	add_definitions(/D _CRT_SECURE_NO_WARNINGS)

	# character set to Not Set=ASCII/SBCS
  # http://public.kitware.com/Bug/view.php?id=12189
	add_definitions(/D _SBCS)
endif()

# disable rtti

if(NOT BUILD_TYPEINFO_IMPL_RTTI)
  if (MSVC)
    add_compile_options(/GR-)
  else()
    add_compile_options(-fno-rtti)
  endif()
endif()

# enable all warnings
if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)

  # fix the code for this
  add_compile_options(-Wno-unused-parameter)
  add_compile_options(-Wno-ignored-qualifiers)
endif()

find_package(SDL2 REQUIRED)
message(STATUS "sdl2 include directory ${SDL2_INCLUDE_DIR}")

# force visual studio and the like to only have debug and release builds
if(CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES Debug Release)
	set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset the configurations to what we need" FORCE)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

if(BUILD_USE_CLANG_TIDY)
  find_program(
    CLANG_TIDY_BINARY
    NAMES "clang-tidy"
    DOC "Path to clang-tidy executable"
  )
  if(NOT CLANG_TIDY_BINARY)
    message(STATUS "clang-tidy not found.")
  else()
    set(CLANG_TIDY_EXE ${CLANG_TIDY_BINARY})
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
  endif()
else()
  message(STATUS "not including clang-tidy in builds")
endif()

if(BUILD_USE_CLANG_FORMAT)
  find_program(
    CLANG_FORMAT_BINARY
    NAMES "clang-format"
    DOC "Path to clang-format executable"
  )
  if(NOT CLANG_FORMAT_BINARY)
    message(STATUS "clang-format not found.")
  else()
    set(CLANG_FORMAT_EXE ${CLANG_FORMAT_BINARY})
    message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")
  endif()
else()
  message(STATUS "not including clang-format in builds")
  unset(CLANG_FORMAT_EXE)
endif()

find_program(
  CLOC_EXE
  NAMES "cloc"
  DOC "Path to cloc executable"
)
if(CLOC_EXE)
  add_custom_target(codestat
                  ${CLOC_EXE} core demo gui proto render tests CMakeLists.txt
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  COMMENT "Running cloc")
endif()

if(BUILD_TYPEINFO_IMPL_CTTI)
  include_directories(SYSTEM external/ctti/include)
endif()

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config_in.h ${CMAKE_CURRENT_BINARY_DIR}/euph_generated_config.h)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(core)

ADD_DEFINITIONS(-DIMGUI_USER_CONFIG="imgui_config.h")
add_executable(binary_to_compressed_c external/imgui/imgui/misc/fonts/binary_to_compressed_c.cpp)

add_subdirectory(base64)
add_subdirectory(render)
add_subdirectory(window)
add_subdirectory(gui)
add_subdirectory(tests)
add_subdirectory(demo)
add_subdirectory(engine)
add_subdirectory(chatbot_test)
add_subdirectory(editor)
add_subdirectory(duk)

if(CLANG_TIDY_EXE)
  ADD_CUSTOM_TARGET(tidy)
  ADD_DEPENDENCIES(tidy
                   tidy-base64
                   tidy-chatbot
                   tidy-core
                   tidy-demo
                   tidy-duk
                   tidy-engine
                   tidy-gui
                   tidy-render
                   tidy-window
                   )
endif()

if(CLANG_FORMAT_EXE)
  ADD_CUSTOM_TARGET(format)
  ADD_DEPENDENCIES(format
                   format-base64
                   format-chatbot
                   format-core
                   format-demo
                   format-duk
                   format-engine
                   format-gui
                   format-render
                   format-window
                   )
endif()
