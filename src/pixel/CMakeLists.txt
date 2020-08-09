set(app_src main.cc)
set(src ${app_src})
source_group("" FILES ${app_src})


include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

add_executable(pixel WIN32 MACOSX_BUNDLE ${src})
target_link_libraries(pixel
                      core
                      render
                      window
                      ${SDL2_LIBRARY}
                      )
