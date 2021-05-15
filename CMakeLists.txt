cmake_minimum_required(VERSION 3.15.0)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
project(euphoria)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake-modules")

option(BUILD_ENTITY_DEBUG_COMPONENT "when converting components, (debug) assert of bad casts" OFF)
set(BUILD_TYPEINFO_IMPL "rtti" CACHE STRING "rtti, ctti, custom_hashed, custom_rt")
set(EXTRA_FLAGS "" CACHE STRING "extra flags to both compile and link")

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


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EXTRA_FLAGS}")

set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/tools/osx-bundle.plist")
set(Gaf_CUSTOM_ARGUMENTS_FROM_FILE ${CMAKE_CURRENT_SOURCE_DIR}/tools/gaf_options.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/external/gaf/gaf.cmake)





# compiler flags
if(${MSVC})
    add_definitions(/D _CRT_SECURE_NO_WARNINGS)

    add_definitions(/D NOMINMAX)
    add_definitions(/D STRICT)
    add_definitions(/D WIN32_LEAN_AND_MEAN)
    add_definitions(/D VC_EXTRALEAN)

    # character set to Not Set=ASCII/SBCS
    # http://public.kitware.com/Bug/view.php?id=12189
    add_definitions(/D _SBCS)

    # range-v3 requires this
    add_compile_options(/permissive-)
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

if(BUILD_TYPEINFO_IMPL_CTTI)
    include_directories(SYSTEM external/ctti/include)
endif()

# todo(Gustav): move to "file" specific config files and not a single global one
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/config_in.h ${CMAKE_CURRENT_BINARY_DIR}/euph_generated_config.h)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DIMGUI_USER_CONFIG="imgui_euphoria_config.h")
add_executable(binary_to_compressed_c external/imgui/imgui/misc/fonts/binary_to_compressed_c.cpp)

set(file_types_folder ${CMAKE_CURRENT_SOURCE_DIR}/libs/file_types/src/file_types)

add_subdirectory(external)
add_subdirectory(libs)
add_subdirectory(apps)

