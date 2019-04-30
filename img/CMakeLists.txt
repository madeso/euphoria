file(GLOB app_src_glob *.cpp;*.cc;*.h;*.inl)

set(app_src ${app_src_glob})
source_group("" FILES ${app_src})

set(all_src ${app_src})

add_executable(img MACOSX_BUNDLE ${all_src})

target_link_libraries(img
                      core
                      )

if(CLANG_TIDY_EXE)
  # -fix
  add_custom_target(tidy-img
                    COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} ${app_src_glob}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if(CLANG_FORMAT_EXE)
  # -fix
  add_custom_target(format-img
                    COMMAND ${CLANG_FORMAT_EXE} -i ${app_src_glob}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
