file(GLOB app_src_glob *.cpp;*.cc;*.h;*.inl)

include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

set(app_src ${app_src_glob})
source_group("" FILES ${app_src})

set(all_src ${app_src} ${src_glew})

# imgui
include_directories(SYSTEM
                    ../external/imgui/
                    )


add_executable(test3d WIN32 MACOSX_BUNDLE ${all_src})

target_link_libraries(test3d
                      core
                      render
                      window
                      ${SDL2_LIBRARY}
                      )
