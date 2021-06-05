################################################################
# header only libraries

add_library(stb_image STATIC stb_image.cc)
target_include_directories(stb_image
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
)

add_library(stb_image_write STATIC stb_image_write.cc)
target_include_directories(stb_image_write
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
)

add_library(stb_rect_pack STATIC stb_rect_pack.cc)
target_include_directories(stb_rect_pack
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
)

add_library(stb_truetype STATIC stb_truetype.cc)
target_include_directories(stb_truetype
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stb
)

add_library(range INTERFACE)
target_include_directories(range INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/range-v3/include)

add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include)

add_library(font8x8 INTERFACE)
target_include_directories(font8x8 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/font8x8)

add_library(font8x13 INTERFACE)
target_include_directories(font8x13 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/font8x13)

add_library(aheasing STATIC
    aheasing/aheasing/easing.c
    aheasing/aheasing/easing.h
)
target_include_directories(aheasing
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/aheasing
)

add_library(crc32 STATIC
    crc32/crc32/crc32.c
    crc32/crc32/crc32.h
)
target_include_directories(crc32
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/crc32
)

add_library(glad STATIC
    glad/src/glad.c
    glad/include/glad/glad.h
)
target_include_directories(glad
    PUBLIC glad/include
)
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})

add_library(icon_font_cpp_headers INTERFACE)
target_include_directories(icon_font_cpp_headers
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/IconFontCppHeaders
)

add_library(catch INTERFACE)
target_include_directories(catch
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/catch
)
target_link_libraries(catch INTERFACE ${CMAKE_THREAD_LIBS_INIT})

add_library(spdlog INTERFACE)
target_include_directories(spdlog
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include
)

add_library(magic_enum INTERFACE)
target_include_directories(magic_enum
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/magic_enum/include
)

################################################################
# more complex libraries

find_package(assimp REQUIRED)
add_library(assimp INTERFACE)
target_include_directories(assimp INTERFACE ${assimp_INCLUDE_DIRS})
target_link_libraries(assimp INTERFACE ${assimp_LIBRARIES})

find_package(OpenGL REQUIRED)
add_library(opengl INTERFACE)
target_link_libraries(opengl INTERFACE ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})

find_package(SDL2 REQUIRED)
add_library(sdl2 INTERFACE)
target_include_directories(sdl2 INTERFACE ${SDL2_INCLUDE_DIR})
target_link_libraries(sdl2 INTERFACE ${SDL2_LIBRARY})

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
target_include_directories(imgui
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
    #PUBLIC
    #    -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS
)
target_link_libraries(imgui PUBLIC core sdl2 glad) # need core for assert override

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
target_include_directories(lua
    PUBLIC
        sol2
        lua/src
)
