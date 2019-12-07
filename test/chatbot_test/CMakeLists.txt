file(GLOB app_src_glob *.cpp;*.cc;*.h;*.inl)

set(app_src ${app_src_glob})
source_group("" FILES ${app_src})

set(all_src ${app_src} ${src_glew})

add_executable(chatbot_test MACOSX_BUNDLE ${all_src})

target_link_libraries(chatbot_test
                      core
                      )
