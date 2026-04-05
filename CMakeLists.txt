cmake_minimum_required(VERSION 3.20.0)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_cmake")
project(euphoria C CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_ENABLE_EXPORTS ON) # for debug builds only...

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake/modules")
include(tools/cmake/standard_project_settings.cmake)
include(tools/cmake/compiler_warnings.cmake)


###################################################################################################
# options
option(GENERATE_COVERAGE "generate code coverage information" OFF)
message(STATUS "Generating coverage: ${GENERATE_COVERAGE}")

option(ENABLE_GL_DEBUG "enable opengl debug information" ON)
message(STATUS "Open GL debug information: ${ENABLE_GL_DEBUG}")

set(DEFAULT_EU_SDL_SOURCE "FindLibrary")
if(WIN32)
    set(DEFAULT_EU_SDL_SOURCE "FetchContent")
endif()
set(EU_SDL_SOURCE "${DEFAULT_EU_SDL_SOURCE}" CACHE STRING "How to get SDL library (FindLibrary or FetchContent)")
set_property(CACHE EU_SDL_SOURCE PROPERTY STRINGS FindLibrary FetchContent)


###################################################################################################
# util

function(target_feature_flags TARGET VIS) # ...targets to enable/disable
    foreach(NAME IN LISTS ARGN)
        if(${${NAME}})
            target_compile_definitions(${TARGET} ${VIS} "-D${NAME}=FF_IS_ENABLED")
        else()
            target_compile_definitions(${TARGET} ${VIS} "-D${NAME}=FF_IS_DISABLED")
        endif()
    endforeach()
endfunction()


###################################################################################################
## global settings applying to all
if(WIN32)
    # Error C1128	number of sections exceeded object file format limit: compile with /bigobj
    # https://developercommunity.visualstudio.com/t/Enable-bigobj-by-default/1031214
    # https://stackoverflow.com/questions/15110580/penalty-of-the-msvs-compiler-flag-bigobj
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()


###################################################################################################
# Validate EU_SDL_SOURCE
if(EU_SDL_SOURCE STREQUAL "FindLibrary")
    set(EU_SDL_SOURCE_FETCH OFF)
    set(EU_SDL_SOURCE_FIND ON)
elseif(EU_SDL_SOURCE STREQUAL "FetchContent")
    set(EU_SDL_SOURCE_FETCH ON)
    set(EU_SDL_SOURCE_FIND OFF)
else()
    message(ERROR "invalid EU_SDL_SOURCE (${EU_SDL_SOURCE})")
endif()

###################################################################################################
## Global build
# enable building on all cores
# https://blog.kitware.com/cmake-building-with-all-your-cores/
# if(${MSVC})
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
# endif()


###################################################################################################
# helper libraries

add_library(edit_and_continue INTERFACE)
if(MSVC)
    # ZI = add debug information with edit and continue support to the target
    # Zi = same with edit and continue disabled
    target_compile_options(edit_and_continue INTERFACE "/ZI")
    target_link_options(edit_and_continue INTERFACE "/INCREMENTAL")
endif()
add_library(euph::edit_and_continue ALIAS edit_and_continue)


###################################################################################################
# root libraries
add_library(project_options INTERFACE)
add_library(euph::project_options ALIAS project_options)
target_link_libraries(project_options INTERFACE euph::edit_and_continue)
target_compile_features(project_options INTERFACE cxx_std_20)
target_feature_flags(project_options INTERFACE
    ENABLE_GL_DEBUG
    BLUR_USE_GAUSS
)


add_library(project_warnings INTERFACE)
add_library(euph::project_warnings ALIAS project_warnings)
set_project_warnings(project_warnings)

source_group("_cmake/rules" REGULAR_EXPRESSION "\\.rule$")
source_group("_cmake" REGULAR_EXPRESSION "CMakeLists\.txt") # is this the correct way to move CMakeLists to a _cmake folder?


if(GENERATE_COVERAGE)
    # inspired by https://github.com/JoakimSoderberg/coveralls-cmake
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        # supported
    else()
        message(FATAL_ERROR "Compiler ${CMAKE_C_COMPILER_ID} is not supported!")
    endif()

    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(FATAL_ERROR "Code coverage results with an optimised (non-Debug) build may be misleading!")
    endif()

    # based on
    # https://github.com/cpp-best-practices/cpp_starter_project/blob/main/cmake/Sanitizers.cmake#L6

    # --coverage is a synonym for -fprofile-arcs -ftest-coverage (when compiling) and -lgcov (when linking).
    # https://gcc.gnu.org/onlinedocs/gcc-6.2.0/gcc/Instrumentation-Options.html
    target_compile_options(project_options INTERFACE -fprofile-arcs -ftest-coverage -O0 -g)
    target_link_libraries(project_options INTERFACE -lgcov)
endif()



set(OpenGL_GL_PREFERENCE "GLVND")

set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/tools/osx-bundle.plist")


###################################################################################################
## Testing
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
enable_testing()


###################################################################################################
## Default folders
set_target_properties(Continuous PROPERTIES FOLDER "_cmake/tests")
set_target_properties(Experimental PROPERTIES FOLDER "_cmake/tests")
set_target_properties(Nightly PROPERTIES FOLDER "_cmake/tests")
set_target_properties(NightlyMemoryCheck PROPERTIES FOLDER "_cmake/tests")



###################################################################################################
# compiler flags
if(${MSVC})
    target_compile_definitions(project_options
        INTERFACE
            _CRT_SECURE_NO_WARNINGS
            NOMINMAX
            STRICT
            WIN32_LEAN_AND_MEAN
            VC_EXTRALEAN

            # character set to Not Set=ASCII/SBCS
            # http://public.kitware.com/Bug/view.php?id=12189
            _SBCS
    )

    # range-v3 requires this
    add_compile_options(/permissive-)
endif()

if(win32)
    message(STATUS "Disabling RTTI")
    if(MSVC)
        add_compile_options(/GR-)
    else()
        add_compile_options(-fno-rtti)
    endif()
else()
    message(STATUS "Linux (debug) requires RTTI?")
endif()



###################################################################################################
# arch detection
set(EU_ARCH_64 OFF)
set(EU_ARCH_32 OFF)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64 bits
    set(EU_ARCH_64 ON)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(EU_ARCH_32 ON)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SIZEOF_VOID_P}")
endif()



# todo(Gustav): move to "file" specific config files and not a single global one or remove in favor of feature flags macro?
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/config_in.h ${CMAKE_CURRENT_BINARY_DIR}/euph_generated_config.h)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DIMGUI_USER_CONFIG="imgui_euphoria_config.h")
add_executable(binary_to_compressed_c external/imgui/imgui/misc/fonts/binary_to_compressed_c.cpp)
set_target_properties(binary_to_compressed_c PROPERTIES FOLDER "Build")


###################################################################################################
## Add projects

set(EUPH_FONTS ${CMAKE_CURRENT_SOURCE_DIR}/data/fonts/)

add_subdirectory(external)
add_subdirectory(libs)
add_subdirectory(apps)



###################################################################################################
# tools to run workbench
add_custom_target(workbench_order_in_file
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/buildtools/Workbench/bin/Debug/net7.0/wb.exe order-in-file check ${CMAKE_CURRENT_SOURCE_DIR}/docs/generated/xml/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(workbench_order_in_file PROPERTIES FOLDER "_tools")

add_custom_target(workbench_check_names
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/buildtools/Workbench/bin/Debug/net7.0/wb.exe check-names check ${CMAKE_CURRENT_SOURCE_DIR}/docs/generated/xml/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(workbench_check_names PROPERTIES FOLDER "_tools")
