cmake_minimum_required(VERSION 3.20.0)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_cmake")
project(euphoria C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake/modules")
include(tools/cmake/standard_project_settings.cmake)
include(tools/cmake/compiler_warnings.cmake)

if(WIN32)
    # Error C1128	number of sections exceeded object file format limit: compile with /bigobj
    # https://developercommunity.visualstudio.com/t/Enable-bigobj-by-default/1031214
    # https://stackoverflow.com/questions/15110580/penalty-of-the-msvs-compiler-flag-bigobj
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

# for debug builds only...
set(CMAKE_ENABLE_EXPORTS ON)

# enable building on all cores
# https://blog.kitware.com/cmake-building-with-all-your-cores/
# if(${MSVC})
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
# endif()

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
add_library(euph::project_options ALIAS project_options)

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)
add_library(euph::project_warnings ALIAS project_warnings)

source_group("_cmake/rules" REGULAR_EXPRESSION "\\.rule$")
source_group("_cmake" REGULAR_EXPRESSION "CMakeLists\.txt") # is this the correct way to move CMakeLists to a _cmake folder?


option(BUILD_ENTITY_DEBUG_COMPONENT "when converting components, (debug) assert of bad casts" OFF)
option(GENERATE_COVERAGE "generate code coverage information" OFF)


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

    message(STATUS "Coverage enabled")

    # based on
    # https://github.com/cpp-best-practices/cpp_starter_project/blob/main/cmake/Sanitizers.cmake#L6

    # --coverage is a synonym for -fprofile-arcs -ftest-coverage (when compiling) and -lgcov (when linking).
    # https://gcc.gnu.org/onlinedocs/gcc-6.2.0/gcc/Instrumentation-Options.html
    target_compile_options(project_options INTERFACE -fprofile-arcs -ftest-coverage -O0 -g)
    target_link_libraries(project_options INTERFACE -lgcov)
endif()

set(OpenGL_GL_PREFERENCE "GLVND")

set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/tools/osx-bundle.plist")
set(Gaf_CUSTOM_ARGUMENTS_FROM_FILE ${CMAKE_CURRENT_SOURCE_DIR}/tools/gaf_options.txt)

include(CTest)
include(external/catch2/extras/Catch.cmake)

enable_testing()

set_target_properties(Continuous PROPERTIES FOLDER "_cmake/tests")
set_target_properties(Experimental PROPERTIES FOLDER "_cmake/tests")
set_target_properties(Nightly PROPERTIES FOLDER "_cmake/tests")
set_target_properties(NightlyMemoryCheck PROPERTIES FOLDER "_cmake/tests")


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



##-----------------------------------------------------------------------------
## arch detection
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
##-----------------------------------------------------------------------------



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

# run workbench
add_custom_target(workbench_order_in_file
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/buildtools/Workbench/bin/Debug/net7.0/wb.exe order-in-file check ${CMAKE_CURRENT_SOURCE_DIR}/docs/generated/xml/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(workbench_check_names
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/buildtools/Workbench/bin/Debug/net7.0/wb.exe check-names check ${CMAKE_CURRENT_SOURCE_DIR}/docs/generated/xml/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
