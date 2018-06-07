file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
SET(Gaf_CUSTOM_PREFIX gaf_)
GAF_GENERATE_CPP(SPRITE_PROTO_SRCS SPRITE_PROTO_HDRS
                 ../file_types/scalingsprite.gaf
                 )
set(src ${src} ${SPRITE_PROTO_SRCS} ${SPRITE_PROTO_HDRS})

include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

include_directories(SYSTEM
                    ../external/imgui/
                    )
include_directories(SYSTEM ../external/IconFontCppHeaders/)

add_executable(editor WIN32 MACOSX_BUNDLE ${src})
target_link_libraries(editor
                      core
                      render
                      window
                      ${SDL2_LIBRARY}
                      )

if(CLANG_TIDY_EXE)
  # -fix
  ADD_CUSTOM_TARGET(tidy-editor
                    COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} ${app_src}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if(CLANG_FORMAT_EXE)
  # -fix
  ADD_CUSTOM_TARGET(format-editor
                    COMMAND ${CLANG_FORMAT_EXE} -i ${app_src}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
