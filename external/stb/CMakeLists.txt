function(add_stb_library STB_NAME)
    add_library(stb_${STB_NAME} STATIC ${STB_NAME}/stb_${STB_NAME}.cc)
    set_target_properties(stb_${STB_NAME} PROPERTIES FOLDER "Dependencies")
    target_include_directories(stb_${STB_NAME} SYSTEM
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${STB_NAME}
    )
    add_library(external::stb_${STB_NAME} ALIAS stb_${STB_NAME})
endfunction(add_stb_library)


add_stb_library(image)
add_stb_library(image_write)
add_stb_library(rect_pack)
add_stb_library(truetype)