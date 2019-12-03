file(GLOB app_src *.cpp;*.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})












include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

add_executable(t3d WIN32 MACOSX_BUNDLE ${src})
target_link_libraries(t3d
                      core
                      render
                      window
                      ${SDL2_LIBRARY}
                      )