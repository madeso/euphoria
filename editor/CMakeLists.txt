file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})

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

include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

add_executable(editor WIN32 MACOSX_BUNDLE ${src})
target_link_libraries(editor
                      core
                      render
                      window
                      ${SDL2_LIBRARY}
                      )
