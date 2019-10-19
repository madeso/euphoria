file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})


include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

add_executable(painter WIN32 MACOSX_BUNDLE ${src})
target_link_libraries(painter
                      core
                      render
                      window
                      ${SDL2_LIBRARY}
                      )
