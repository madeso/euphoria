cmake_minimum_required(VERSION 3.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(euphoria)

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

if(${APPLE})
  # disabling http://stackoverflow.com/questions/32626171/xcode-7-how-to-suppress-warning-overrides-a-member-function-but-is-not-marked
  # that apple-clang spews out for wx headers
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-inconsistent-missing-override")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

# compiler flags
if(${MSVC})
	add_definitions(/D _CRT_SECURE_NO_WARNINGS)

	# character set to Not Set=ASCII/SBCS
  # http://public.kitware.com/Bug/view.php?id=12189
	add_definitions(/D _SBCS)
endif()

# disable rtti
if (MSVC)
  add_compile_options(/GR-)
else()
  add_compile_options(-fno-rtti)
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

find_program(
  CLANG_TIDY_EXE
  NAMES "clang-tidy"
  DOC "Path to clang-tidy executable"
)
if(NOT CLANG_TIDY_EXE)
  message(STATUS "clang-tidy not found.")
else()
  message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
endif()

find_program(
  CLANG_FORMAT_EXE
  NAMES "clang-format"
  DOC "Path to clang-format executable"
)
if(NOT CLANG_FORMAT_EXE)
  message(STATUS "clang-format not found.")
else()
  message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")
endif()

find_program(
  CLOC_EXE
  NAMES "cloc"
  DOC "Path to cloc executable"
)
if(CLOC_EXE)
  add_custom_target(codestat
                  ${CLOC_EXE} core demo gui proto render scimed tests CMakeLists.txt
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  COMMENT "Running cloc")
endif()

include_directories(SYSTEM external/ctti/include)

add_subdirectory(core)

ADD_DEFINITIONS(-DIMGUI_USER_CONFIG="imgui_config.h")
add_executable(binary_to_compressed_c external/imgui/imgui/misc/fonts/binary_to_compressed_c.cpp)

add_subdirectory(base64)
add_subdirectory(render)
add_subdirectory(window)
add_subdirectory(gui)
add_subdirectory(scimed)
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
                   tidy-scimed
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
                   format-scimed
                   format-window
                   )
endif()
