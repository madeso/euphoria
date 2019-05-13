file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})


include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

include_directories(SYSTEM
                    ../external/imgui/
                    )
include_directories(SYSTEM ../external/IconFontCppHeaders/)

add_executable(painter WIN32 MACOSX_BUNDLE ${src})
target_link_libraries(painter
                      core
                      render
                      window
                      ${SDL2_LIBRARY}
                      )

if(CLANG_TIDY_EXE)
  # -fix
  ADD_CUSTOM_TARGET(tidy-painter
                    COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} ${app_src}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if(CLANG_FORMAT_EXE)
  # -fix
  ADD_CUSTOM_TARGET(format-painter
                    COMMAND ${CLANG_FORMAT_EXE} -i ${app_src}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
