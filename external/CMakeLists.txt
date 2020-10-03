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


################################################################
# more complex libraries

find_package(Freetype REQUIRED)
add_library(freetype INTERFACE)
target_include_directories(freetype INTERFACE ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(freetype INTERFACE ${FREETYPE_LIBRARIES})

add_library(assimp INTERFACE)
target_include_directories(assimp INTERFACE ${assimp_INCLUDE_DIRS})
target_link_libraries(assimp INTERFACE ${assimp_LIBRARIES})

