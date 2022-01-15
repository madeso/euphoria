###################################################################################################
## fmt
add_subdirectory(fmt-7.1.1)




###################################################################################################
## gaf
add_subdirectory(gaf)
set_target_properties(fmt PROPERTIES FOLDER "Dependencies")




###################################################################################################
## stb image
add_library(stb_image STATIC stb_image.cc)
set_target_properties(stb_image PROPERTIES FOLDER "Dependencies")
target_include_directories(stb_image SYSTEM
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
)




###################################################################################################
## stb image write
add_library(stb_image_write STATIC stb_image_write.cc)
set_target_properties(stb_image_write PROPERTIES FOLDER "Dependencies")
target_include_directories(stb_image_write SYSTEM
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
)




###################################################################################################
## stb rect pack
add_library(stb_rect_pack STATIC stb_rect_pack.cc)
set_target_properties(stb_rect_pack PROPERTIES FOLDER "Dependencies")
target_compile_definitions(stb_rect_pack
    # required for using int in packing
    PUBLIC STBRP_LARGE_RECTS
)
target_include_directories(stb_rect_pack SYSTEM
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
)




###################################################################################################
## stb true type
add_library(stb_truetype STATIC stb_truetype.cc)
set_target_properties(stb_truetype PROPERTIES FOLDER "Dependencies")
target_include_directories(stb_truetype SYSTEM
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
)




###################################################################################################
## range
# todo(Gustav): remove once we update to c++20
add_library(range INTERFACE)
target_include_directories(range SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/range-v3/include
)




###################################################################################################
## font 8x8
add_library(font8x8 INTERFACE)
target_include_directories(font8x8  SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/font8x8
)




###################################################################################################
## font 8x13
add_library(font8x13 INTERFACE)
target_include_directories(font8x13 SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/font8x13
)




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




###################################################################################################
## icon font cpp headers
add_library(icon_font_cpp_headers INTERFACE)
target_include_directories(icon_font_cpp_headers SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/IconFontCppHeaders
)




###################################################################################################
## catch
add_library(catch INTERFACE)
target_include_directories(catch SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/catch
)
target_link_libraries(catch INTERFACE ${CMAKE_THREAD_LIBS_INIT})




###################################################################################################
## spdlog
add_library(spdlog INTERFACE)
target_include_directories(spdlog SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include
)



###################################################################################################
## magic enum
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/magic_enum/include
)



###################################################################################################
## assimp
find_package(assimp REQUIRED)
add_library(assimp INTERFACE)
target_include_directories(assimp  SYSTEM
    INTERFACE ${assimp_INCLUDE_DIRS}
)
target_link_libraries(assimp INTERFACE ${assimp_LIBRARIES})



###################################################################################################
## OpenGL
find_package(OpenGL REQUIRED)
add_library(opengl INTERFACE)
target_link_libraries(opengl INTERFACE ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})




###################################################################################################
## sdl2
find_package(SDL2 REQUIRED)
add_library(sdl2 INTERFACE)
target_include_directories(sdl2 SYSTEM
    INTERFACE ${SDL2_INCLUDE_DIR}
)
target_link_libraries(sdl2 INTERFACE ${SDL2_LIBRARY})




###################################################################################################
## DearIMGUI
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
    imgui/imgui/backends/imgui_impl_sdl.cpp
    imgui/imgui/backends/imgui_impl_sdl.h
    imgui/imgui/backends/imgui_impl_opengl3.cpp
    imgui/imgui/backends/imgui_impl_opengl3.h
)
set_target_properties(imgui PROPERTIES FOLDER "Dependencies")
target_include_directories(imgui SYSTEM
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/
    # we want this to be private but we currently include the sdl loader
    # and it include imgui the 'imgui way'
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/fake/
)
target_compile_definitions(imgui
    PRIVATE
        -DIMGUI_IMPL_OPENGL_LOADER_GLAD
    PUBLIC
        -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS
)
target_link_libraries(imgui
    PUBLIC
        sdl2
        glad
        # need core and project_options for assert override
        core
        project_options
)




###################################################################################################
## lua
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
