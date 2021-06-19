###############################################################################
# editor project

set(src
    src/editor/browser.cc src/editor/browser.h
    src/editor/main.cc
    src/editor/scimed.cc src/editor/scimed.h
)
source_group("" FILES ${src})

# file_types is only plain gaf
# when gaf suppors several outputs, link with a file_types_imgui library instead
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
SET(Gaf_CUSTOM_PREFIX gaf_)
SET(Gaf_INCLUDE_IMGUI Yes)
GAF_GENERATE_CPP(SPRITE_PROTO_SRCS SPRITE_PROTO_HDRS
    ${file_types_folder}/scalingsprite.gaf
    ${file_types_folder}/game.gaf
    ${file_types_folder}/world.gaf
    ${file_types_folder}/enum.gaf
)
set(src ${src} ${SPRITE_PROTO_SRCS} ${SPRITE_PROTO_HDRS})


add_executable(editor WIN32 MACOSX_BUNDLE ${src})
set_target_properties(editor PROPERTIES FOLDER "Applications")
target_include_directories(editor
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(editor
    PRIVATE
        project_options
        project_warnings
        core
        render
        window
        sdl2
        imgui
)

