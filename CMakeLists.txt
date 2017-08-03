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

if(EUPHORIA_TEST)
  find_package(GMock REQUIRED)
endif(EUPHORIA_TEST)

FIND_PACKAGE(Threads REQUIRED) # for gmock

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

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

add_subdirectory(core)
add_subdirectory(render)
add_subdirectory(gui)
add_subdirectory(scimed)

if(EUPHORIA_TEST)
  add_subdirectory(tests)
endif(EUPHORIA_TEST)

add_subdirectory(demo)