cmake_minimum_required(VERSION 3.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(euphoria)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

find_package(OpenGL REQUIRED)

find_package(Protobuf REQUIRED)
include_directories(SYSTEM ${PROTOBUF_INCLUDE_DIRS})

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/external/gsl-lite/include)

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


# enable all warnings
# https://stackoverflow.com/questions/1610030/why-does-flowing-off-the-end-of-a-non-void-function-without-returning-a-value-no
# https://stackoverflow.com/questions/2368811/how-to-set-warning-level-in-cmake
if(MSVC)
	# Force to always compile with W4
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	# Update if necessary
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
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

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
set(pbjson_src
		${CMAKE_CURRENT_SOURCE_DIR}/external/pbjson/src/pbjson.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/external/pbjson/src/pbjson.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/external/pbjson/src/bin2ascii.h
		)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/pbjson/src/)


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

add_subdirectory(core)
add_subdirectory(base64)
add_subdirectory(render)
add_subdirectory(window)
# add_subdirectory(gui)
add_subdirectory(scimed)
add_subdirectory(tests)
add_subdirectory(demo)

if(CLANG_TIDY_EXE)
  ADD_CUSTOM_TARGET(tidy)
  ADD_DEPENDENCIES(tidy
                   tidy-core
                   tidy-render
                   )
endif()

if(CLANG_FORMAT_EXE)
  ADD_CUSTOM_TARGET(format)
  ADD_DEPENDENCIES(format
                   format-core
                   format-render
                   format-demo
                   )
endif()
