add_subdirectory(test3d)

file(GLOB app_src_glob *.cpp;*.cc;*.h;*.inl)
source_group("" FILES ${app_src_glob})


add_executable(base64 base64.cc)
target_link_libraries(base64 core)

add_executable(chatbot chatbot.cc)
target_link_libraries(chatbot core)

add_executable(img img.cc)
target_link_libraries(img core)

add_executable(markov markov.cc)
target_link_libraries(markov core)

add_executable(svg-dump svg-dump.cc)
target_link_libraries(svg-dump core)

add_executable(tabletool tabletool.cc)
target_link_libraries(tabletool core)

add_executable(three-dump three-dump.cc)
target_link_libraries(three-dump core)

add_executable(tracery tracery.cc)
target_link_libraries(tracery core)

add_executable(tree tree.cc)
target_link_libraries(tree core)

add_executable(worldgen worldgen.cc)
target_link_libraries(worldgen core)
