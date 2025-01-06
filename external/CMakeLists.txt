Include(FetchContent)
set(CATCH_GIT_TAG v3.7.1)




###################################################################################################
## undef_windows
add_subdirectory(undef_windows)
set_target_properties(undef_windows PROPERTIES FOLDER "Dependencies")





###################################################################################################
## json
add_subdirectory(json)
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
## stb
# stb image
add_library(stb_image STATIC stb_image.cc)
set_target_properties(stb_image PROPERTIES FOLDER "Dependencies")
target_include_directories(stb_image SYSTEM
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
)
add_library(external::stb_image ALIAS stb_image)


# stb image write
add_library(stb_image_write STATIC stb_image_write.cc)
set_target_properties(stb_image_write PROPERTIES FOLDER "Dependencies")
target_include_directories(stb_image_write SYSTEM
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
)
add_library(external::stb_image_write ALIAS stb_image_write)


# stb rect pack
add_library(stb_rect_pack STATIC stb_rect_pack.cc)
set_target_properties(stb_rect_pack PROPERTIES FOLDER "Dependencies")
target_compile_definitions(stb_rect_pack
    # required for using int in packing
    PUBLIC STBRP_LARGE_RECTS
)
target_include_directories(stb_rect_pack SYSTEM
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
)
add_library(external::stb_rect_pack ALIAS stb_rect_pack)


# stb true type
add_library(stb_truetype STATIC stb_truetype.cc)
set_target_properties(stb_truetype PROPERTIES FOLDER "Dependencies")
target_include_directories(stb_truetype SYSTEM
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
)
add_library(external::stb_truetype ALIAS stb_truetype)



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
    PUBLIC glad/include
)
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

###################################################################################################
## fmt
add_subdirectory(fmt-10.1.1)
add_library(fmt_dep INTERFACE)
target_link_libraries(fmt_dep INTERFACE fmt::fmt)
add_library(external::fmt ALIAS fmt_dep)



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
## assimp
find_package(assimp REQUIRED)
add_library(assimp INTERFACE)
target_include_directories(assimp  SYSTEM
    INTERFACE ${assimp_INCLUDE_DIRS}
)
target_link_libraries(assimp INTERFACE ${assimp_LIBRARIES})
add_library(external::assimp ALIAS assimp)



###################################################################################################
## OpenGL
find_package(OpenGL REQUIRED)
add_library(opengl INTERFACE)
target_link_libraries(opengl INTERFACE ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})
add_library(external::opengl ALIAS opengl)



###################################################################################################
## sdl2
find_package(SDL2 REQUIRED)
add_library(sdl2 INTERFACE)
target_include_directories(sdl2 SYSTEM
    INTERFACE ${SDL2_INCLUDE_DIR}
)
target_link_libraries(sdl2 INTERFACE ${SDL2_LIBRARY})
add_library(external::sdl2 ALIAS sdl2)



###################################################################################################
## DearIMGUI

# todo(Gustav): investigate enabling ImGui_ImplWin32_EnableDpiAwareness

add_library(imgui STATIC
    imgui/imgui/imconfig.h
    imgui/imgui/imgui_demo.cpp
    imgui/imgui/imgui_draw.cpp
    imgui/imgui/imgui_internal.h
    imgui/imgui/imgui_tables.cpp
    imgui/imgui/imgui_widgets.cpp
    imgui/imgui/imgui.cpp
    imgui/imgui/imgui.h
    # todo: look into how different stb libs work together
    imgui/imgui/imstb_rectpack.h
    imgui/imgui/imstb_textedit.h
    imgui/imgui/imstb_truetype.h

    imgui/imgui/misc/cpp/imgui_stdlib.cpp
    imgui/imgui/misc/cpp/imgui_stdlib.h

    imgui/imgui/backends/imgui_impl_sdl.cpp
    imgui/imgui/backends/imgui_impl_sdl.h
    imgui/imgui/backends/imgui_impl_opengl3.cpp
    imgui/imgui/backends/imgui_impl_opengl3.h

    imgui/imgui/backends/imgui_impl_opengl3_loader.h # 1.84
)
set_target_properties(imgui PROPERTIES FOLDER "Dependencies")
target_include_directories(imgui SYSTEM
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/misc/cpp/
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/backends
    # we want this to be private but we currently include the sdl loader
    # and it include imgui the 'imgui way'
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/fake/
)
target_compile_definitions(imgui
    PRIVATE
        -DIMGUI_IMPL_OPENGL_LOADER_GLAD
    PUBLIC
        -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS
        # this does not switch to utf-32, this only extends dear imgui to support more fonts 32
        -DIMGUI_USE_WCHAR32
)
target_link_libraries(imgui
    PUBLIC
        sdl2
        glad
        # need assert and project_options for assert override
        assert
        project_options
)
add_library(external::imgui ALIAS imgui)



###################################################################################################
## imguizmo
add_library(imguizmo STATIC
    imguizmo/imguizmo/ImGuizmo.cpp
    imguizmo/imguizmo/ImGuizmo.h
)
set_target_properties(imguizmo PROPERTIES FOLDER "Dependencies")
target_include_directories(imguizmo SYSTEM
    PUBLIC
        imguizmo
)
target_link_libraries(imguizmo
    PUBLIC
        external::imgui
)
add_library(external::imguizmo ALIAS imguizmo)




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
