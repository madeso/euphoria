cmake_minimum_required(VERSION 3.20.0)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_cmake")
project(euphoria C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake/modules")
include(tools/cmake/standard_project_settings.cmake)
include(tools/cmake/compiler_warnings.cmake)

set(CMAKE_CXX_EXTENSIONS OFF)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

source_group("_cmake/rules" REGULAR_EXPRESSION "\\.rule$")
source_group("_cmake" REGULAR_EXPRESSION "CMakeLists\.txt") # is this the correct way to move CMakeLists to a _cmake folder?


option(BUILD_ENTITY_DEBUG_COMPONENT "when converting components, (debug) assert of bad casts" OFF)
set(BUILD_TYPEINFO_IMPL "rtti" CACHE STRING "rtti, ctti, custom_hashed, custom_rt")
option(GENERATE_COVERAGE "generate code coverage information" OFF)


if(GENERATE_COVERAGE)
    # inspired by https://github.com/JoakimSoderberg/coveralls-cmake
    if(NOT (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        AND (NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang"))
        message(FATAL_ERROR "Coveralls: Compiler ${CMAKE_C_COMPILER_ID} is not GNU gcc! Aborting... You can set this on the command line using CC=/usr/bin/gcc CXX=/usr/bin/g++ cmake <options> ..")
    endif()

    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(FATAL_ERROR "Coveralls: Code coverage results with an optimised (non-Debug) build may be misleading! Add -DCMAKE_BUILD_TYPE=Debug")
    endif()

    # is -fprofile-generate needed?
    target_compile_options(project_options " -fprofile-arcs -ftest-coverage -fprofile-dir=${CMAKE_CURRENT_BINARY_DIR}/coverage -fprofile-generate")
    target_link_libraries(project_options  "-lgcov --coverage")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -fprofile-dir=${CMAKE_CURRENT_BINARY_DIR}/coverage -fprofile-generate")
    # set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fprofile-arcs -ftest-coverage -fprofile-dir=${CMAKE_CURRENT_BINARY_DIR}/coverage -fprofile-generate")
    # set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -fprofile-dir=${CMAKE_CURRENT_BINARY_DIR}/coverage -fprofile-generate")
endif()

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

set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/tools/osx-bundle.plist")
set(Gaf_CUSTOM_ARGUMENTS_FROM_FILE ${CMAKE_CURRENT_SOURCE_DIR}/tools/gaf_options.txt)

include(CTest)
include(external/catch/Catch.cmake)

enable_testing()

set_target_properties(Continuous PROPERTIES FOLDER "_cmake/tests")
set_target_properties(Experimental PROPERTIES FOLDER "_cmake/tests")
set_target_properties(Nightly PROPERTIES FOLDER "_cmake/tests")
set_target_properties(NightlyMemoryCheck PROPERTIES FOLDER "_cmake/tests")


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


if(BUILD_TYPEINFO_IMPL_CTTI)
    include_directories(SYSTEM external/ctti/include)
endif()

# todo(Gustav): move to "file" specific config files and not a single global one
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/config_in.h ${CMAKE_CURRENT_BINARY_DIR}/euph_generated_config.h)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DIMGUI_USER_CONFIG="imgui_euphoria_config.h")
add_executable(binary_to_compressed_c external/imgui/imgui/misc/fonts/binary_to_compressed_c.cpp)
set_target_properties(binary_to_compressed_c PROPERTIES FOLDER "Build")

set(file_types_folder ${CMAKE_CURRENT_SOURCE_DIR}/libs/file_types/src/file_types)

include(tools/cmake/generate-header-test.cmake)

add_subdirectory(external)
add_subdirectory(libs)
add_subdirectory(apps)
