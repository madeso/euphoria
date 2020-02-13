file(GLOB app_src_glob *.cpp;*.cc;*.h;*.inl)
source_group("" FILES ${app_src_glob})

# single file applications that could be useful

add_executable(tabletool tabletool.cc)
target_link_libraries(tabletool core)

add_executable(base64 base64.cc)
target_link_libraries(base64 core)

add_executable(markov markov.cc)
target_link_libraries(markov core)

add_executable(tracery tracery.cc)
target_link_libraries(tracery core)




# test applications that serve no purpose other than
# testing the libraries and perhaps generate some pretty visuals :)

# set(all_src ${app_src} ${src_glew})

add_executable(test-3d WIN32 MACOSX_BUNDLE 3d.cc)
target_include_directories(test-3d SYSTEM PRIVATE ${SDL2_INCLUDE_DIR})
target_link_libraries(test-3d core render window ${SDL2_LIBRARY})


add_executable(test-chatbot chatbot.cc)
target_link_libraries(test-chatbot core)

add_executable(test-coldet coldet.cc)
target_link_libraries(test-coldet core)

add_executable(test-draw-test draw-test.cc)
target_link_libraries(test-draw-test core)

add_executable(test-img img.cc)
target_link_libraries(test-img core)

add_executable(test-poisson-test poisson-test.cc)
target_link_libraries(test-poisson-test core)

add_executable(test-svg-dump svg-dump.cc)
target_link_libraries(test-svg-dump core)

add_executable(test-three-dump three-dump.cc)
target_link_libraries(test-three-dump core)

add_executable(test-identicon identicon-main.cc)
target_link_libraries(test-identicon core)

add_executable(test-canvas canvas-main.cc)
target_link_libraries(test-canvas core)

add_executable(test-tree tree.cc)
target_link_libraries(test-tree core)

add_executable(test-worldgen worldgen.cc)
target_link_libraries(test-worldgen core)

add_executable(test-voronoi voronoi.cc)
target_link_libraries(test-voronoi core)

