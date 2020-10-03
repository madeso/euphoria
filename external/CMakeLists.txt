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

# todo(Gustav): replace duktape with either lua or custom scriping engine fel
# release date 2017-09-23 version duktape-2.2.0.tar.xz
add_library(duktape STATIC
    duktape/duktape/duktape.c
)
target_include_directories(duktape
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/duktape
)


add_library(glad STATIC
    glad/src/glad.c
    glad/include/glad/glad.h
)
target_include_directories(glad
    PUBLIC glad/include
)

################################################################
# more complex libraries

find_package(Freetype REQUIRED)
add_library(freetype INTERFACE)
target_include_directories(freetype INTERFACE ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(freetype INTERFACE ${FREETYPE_LIBRARIES})

add_library(assimp INTERFACE)
target_include_directories(assimp INTERFACE ${assimp_INCLUDE_DIRS})
target_link_libraries(assimp INTERFACE ${assimp_LIBRARIES})

add_library(opengl INTERFACE)
target_link_libraries(opengl INTERFACE ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS})

