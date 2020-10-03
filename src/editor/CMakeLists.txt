set(app_src
    browser.cc
    browser.h
    main.cc
    scimed.cc
    scimed.h
)
set(src ${app_src})
source_group("" FILES ${app_src})

# file_types is only plain gaf
# when gaf suppors several outputs, link with a file_types_imgui library instead
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
SET(Gaf_CUSTOM_PREFIX gaf_)
SET(Gaf_INCLUDE_IMGUI Yes)
GAF_GENERATE_CPP(SPRITE_PROTO_SRCS SPRITE_PROTO_HDRS
    ../file_types/scalingsprite.gaf
    ../file_types/game.gaf
    ../file_types/world.gaf
    ../file_types/enum.gaf
)
set(src ${src} ${SPRITE_PROTO_SRCS} ${SPRITE_PROTO_HDRS})


add_executable(editor WIN32 MACOSX_BUNDLE ${src})
target_link_libraries(editor
    PRIVATE
    core
    render
    window
    sdl2
    imgui
)

