Include(FetchContent)
include(ExternalProject)

# todo(Gustav): update to sdl 3
set(SDL_VERSION_STRING "2.32.10") # tar.gz name on sdl github release page
set(CATCH_GIT_TAG v3.7.1)




###################################################################################################
## undef_windows
add_subdirectory(undef_windows)
set_target_properties(undef_windows PROPERTIES FOLDER "Dependencies")





###################################################################################################
## embed
add_subdirectory(embed)





###################################################################################################
## stb
add_subdirectory(stb)





###################################################################################################
## json
add_subdirectory(json)
set_target_properties(jsonh PROPERTIES FOLDER "Dependencies")
# set_target_properties(undef_windows PROPERTIES FOLDER "Dependencies")



###################################################################################################
## guid
add_library(guid INTERFACE)
# using pkg-config to configure uuid
if(WIN32)
    # no work is needed
else()
    find_package(PkgConfig REQUIRED)
    find_package(Threads REQUIRED)
    pkg_search_module(UUID REQUIRED uuid)
    target_include_directories(guid SYSTEM INTERFACE ${UUID_INCLUDE_DIRS})
    target_link_libraries(guid INTERFACE ${UUID_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()
add_library(external::guid ALIAS guid)



###################################################################################################
## range v3
# todo(Gustav): remove once we update to c++20
add_library(range INTERFACE)
target_include_directories(range SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/range-v3/include
)
add_library(external::range ALIAS range)



###################################################################################################
## font 8x8
add_library(font8x8 INTERFACE)
target_include_directories(font8x8  SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/font8x8
)
add_library(external::font8x8 ALIAS font8x8)



###################################################################################################
## font 8x13
add_library(font8x13 INTERFACE)
target_include_directories(font8x13 SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/font8x13
)
add_library(external::font8x13 ALIAS font8x13)



###################################################################################################
## crc32
add_library(crc32 STATIC
    crc32/crc32/crc32.c
    crc32/crc32/crc32.h
)
set_target_properties(crc32 PROPERTIES FOLDER "Dependencies")
target_include_directories(crc32 SYSTEM
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/crc32
)
add_library(external::crc32 ALIAS crc32)



###################################################################################################
## glad
add_library(glad STATIC
    glad/src/glad.c
    glad/include/glad/glad.h
)
set_target_properties(glad PROPERTIES FOLDER "Dependencies")
target_include_directories(glad SYSTEM
    PUBLIC
        glad/include
        glad/extra
)
target_link_libraries(glad PUBLIC undef_windows::undef_windows)
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})
add_library(external::glad ALIAS glad)



###################################################################################################
## icon font cpp headers
add_library(icon_font_cpp_headers INTERFACE)
target_include_directories(icon_font_cpp_headers SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/IconFontCppHeaders
)
add_library(external::icon_font_cpp_headers ALIAS icon_font_cpp_headers)



###################################################################################################
## catch
FetchContent_Declare(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        ${CATCH_GIT_TAG}
)
FetchContent_MakeAvailable(Catch2)
add_library(catch_main STATIC catch2/main.cc)
target_link_libraries(catch_main PUBLIC
    Catch2::Catch2
    euph::assert
    euph::project_options
    euph::project_warnings
)
add_library(external::catch ALIAS catch_main)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
set_target_properties(catch_main PROPERTIES FOLDER "Dependencies/catch2")
set_target_properties(Catch2 PROPERTIES FOLDER "Dependencies/catch2")
set_target_properties(Catch2WithMain PROPERTIES FOLDER "Dependencies/catch2")

###################################################################################################
## fmt
add_subdirectory(fmt-10.1.1)
add_library(fmt_dep INTERFACE)
target_link_libraries(fmt_dep INTERFACE fmt::fmt)
add_library(external::fmt ALIAS fmt_dep)
set_target_properties(fmt PROPERTIES FOLDER "Dependencies")


###################################################################################################
## magic enum
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/magic_enum/include
)
add_library(external::magic_enum ALIAS magic_enum)



###################################################################################################
## open color
add_library(open_color INTERFACE)
target_include_directories(open_color SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/open-color/include
)
add_library(external::open_color ALIAS open_color)



###################################################################################################
## OpenGL
find_package(OpenGL REQUIRED)
add_library(opengl INTERFACE)
target_link_libraries(opengl INTERFACE ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})
add_library(external::opengl ALIAS opengl)



###################################################################################################
## sdl2

###################################################################################################
# SDL2
###################################################################################################

if(EU_SDL_SOURCE_FETCH)
    set(SDL_EXTERN_DIR ${CMAKE_CURRENT_BINARY_DIR}/sdl)
    set(SDL_INSTALL_DIR "${SDL_EXTERN_DIR}/install")
    message(STATUS "Will download and build ${SDL_VERSION_STRING} to ${SDL_EXTERN_DIR} and install to ${SDL_INSTALL_DIR} when building")

    ExternalProject_Add(
        SDL2_external
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VERSION_STRING}/SDL2-${SDL_VERSION_STRING}.zip
        PREFIX "${SDL_EXTERN_DIR}/${SDL_VERSION_STRING}"
        LOG_DIR "${SDL_EXTERN_DIR}/log"
        INSTALL_DIR ${SDL_INSTALL_DIR}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DLIBC=ON
            -DSDL_STATIC=ON
            -DSDL_SHARED=OFF
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON
    )
    set_target_properties(SDL2_external PROPERTIES FOLDER "Dependencies")

    add_library(external_sdl2 INTERFACE)
    add_dependencies(external_sdl2 SDL2_external)
    target_include_directories(external_sdl2 SYSTEM
        INTERFACE
            ${SDL_INSTALL_DIR}/include/SDL2
    )
    if(WIN32)
        target_link_libraries(external_sdl2 INTERFACE
            optimized ${SDL_INSTALL_DIR}/lib/SDL2main.lib
            optimized ${SDL_INSTALL_DIR}/lib/SDL2-static.lib
            debug ${SDL_INSTALL_DIR}/lib/SDL2maind.lib
            debug ${SDL_INSTALL_DIR}/lib/SDL2-staticd.lib
            version.lib imm32.lib winmm.lib Setupapi.lib
        )
    else()
        # todo(Gustav): specify debug flag?
        target_link_libraries(external_sdl2 INTERFACE ${SDL_INSTALL_DIR}/lib/libSDL2.a)
    endif()
    add_library(external::sdl2 ALIAS external_sdl2)
endif()

if(EU_SDL_SOURCE_FIND)
    find_package(SDL2 REQUIRED)
    add_library(external_sdl2 INTERFACE)
    target_include_directories(external_sdl2 SYSTEM
        INTERFACE
            ${SDL2_INCLUDE_DIR}
    )
    target_link_libraries(external_sdl2
        INTERFACE
            ${SDL2_LIBRARY}
            ${CMAKE_DL_LIBS}
    )
    if(WIN32)
    target_link_libraries(external_sdl2
        INTERFACE
        Setupapi.lib
    )
    endif()
    add_library(external::sdl2 ALIAS external_sdl2)
endif()



###################################################################################################
## DearIMGUI

# todo(Gustav): investigate enabling ImGui_ImplWin32_EnableDpiAwareness

set(dear_imgui_src
    dear_imgui/dear_imgui/imconfig.h
    dear_imgui/dear_imgui/imgui_demo.cpp
    dear_imgui/dear_imgui/imgui_draw.cpp
    dear_imgui/dear_imgui/imgui_internal.h
    dear_imgui/dear_imgui/imgui_tables.cpp
    dear_imgui/dear_imgui/imgui_widgets.cpp
    dear_imgui/dear_imgui/imgui.cpp
    dear_imgui/dear_imgui/imgui.h
    dear_imgui/dear_imgui/imstb_rectpack.h
    dear_imgui/dear_imgui/imstb_textedit.h
    dear_imgui/dear_imgui/imstb_truetype.h

    dear_imgui/dear_imgui/backends/imgui_impl_sdl2.cpp
    dear_imgui/dear_imgui/backends/imgui_impl_sdl2.h
    dear_imgui/dear_imgui/backends/imgui_impl_opengl3.cpp
    dear_imgui/dear_imgui/backends/imgui_impl_opengl3.h
)
add_library(dear_imgui STATIC ${dear_imgui_src})
set_target_properties(dear_imgui PROPERTIES FOLDER "Dependencies")
source_group("" FILES ${dear_imgui_src})
target_include_directories(dear_imgui SYSTEM
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/dear_imgui/misc/cpp/
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/dear_imgui/backends
    # we want this to be private but we currently include the sdl loader
    # and it include dear_imgui the 'imgui way'
    ${CMAKE_CURRENT_SOURCE_DIR}/dear_imgui/fake/
)
target_link_libraries(dear_imgui
    PUBLIC
        external::sdl2
        external::glad
        # need assert and project_options for assert override
        euph::assert
        euph::project_options
)
target_compile_definitions(dear_imgui
    PRIVATE
        -DIMGUI_IMPL_OPENGL_LOADER_GLAD
    PUBLIC
        -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS
        -DIMGUI_DEFINE_MATH_OPERATORS
        # this does not switch to utf-32, this only extends dear imgui to support more fonts 32
        -DIMGUI_USE_WCHAR32
)
add_library(external::dear_imgui ALIAS dear_imgui)



###################################################################################################
## imguizmo
# add_library(imguizmo STATIC
#     imguizmo/imguizmo/ImGuizmo.cpp
#     imguizmo/imguizmo/ImGuizmo.h
# )
# set_target_properties(imguizmo PROPERTIES FOLDER "Dependencies")
# target_include_directories(imguizmo SYSTEM
#     PUBLIC
#         imguizmo
# )
# target_link_libraries(imguizmo
#     PUBLIC
#         external::dear_imgui
# )
# add_library(external::imguizmo ALIAS imguizmo)




###################################################################################################
## lua
## sol
add_library(lua STATIC
    lua/src/lapi.c
    lua/src/lauxlib.c
    lua/src/lbaselib.c
    lua/src/lbitlib.c
    lua/src/lcode.c
    lua/src/lcorolib.c
    lua/src/lctype.c
    lua/src/ldblib.c
    lua/src/ldebug.c
    lua/src/ldo.c
    lua/src/ldump.c
    lua/src/lfunc.c
    lua/src/lgc.c
    lua/src/linit.c
    lua/src/liolib.c
    lua/src/llex.c
    lua/src/lmathlib.c
    lua/src/lmem.c
    lua/src/loadlib.c
    lua/src/lobject.c
    lua/src/lopcodes.c
    lua/src/loslib.c
    lua/src/lparser.c
    lua/src/lstate.c
    lua/src/lstring.c
    lua/src/lstrlib.c
    lua/src/ltable.c
    lua/src/ltablib.c
    lua/src/ltm.c
    lua/src/lundump.c
    lua/src/lutf8lib.c
    lua/src/lvm.c
    lua/src/lzio.c
)
set_target_properties(lua PROPERTIES FOLDER "Dependencies")
target_include_directories(lua SYSTEM
    PUBLIC
        sol2
        lua/src
)
add_library(external::lua ALIAS lua)




###################################################################################################
# mustache
add_library(mustache INTERFACE)
target_include_directories(mustache SYSTEM
    INTERFACE
        mustache
)
add_library(external::mustache ALIAS mustache)

