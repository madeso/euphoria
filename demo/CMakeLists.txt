file(GLOB app_src_glob *.cpp;*.cc;*.h;*.inl)

include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

set(app_src ${app_src_glob})
source_group("" FILES ${app_src})

set(all_src ${app_src} ${src_glew})

# imgui
include_directories(SYSTEM
                    ../external/imgui/
                    )


add_executable(demo WIN32 MACOSX_BUNDLE ${all_src})

target_link_libraries(demo
                      core
                      render
                      window
                      ${SDL2_LIBRARY}
                      )
MESSAGE(STATUS "SDL2 libraries are ${SDL2_LIBRARY}")
MESSAGE(STATUS "SDL2 main library is ${SDL2MAIN_LIBRARY}")

if(CMAKE_COMPILER_IS_GNUCC)
  set_property(TARGET demo APPEND_STRING PROPERTY COMPILE_FLAGS -Wall)
endif()

if(CLANG_FORMAT_EXE)
  # -fix
  ADD_CUSTOM_TARGET(format-demo
                    COMMAND ${CLANG_FORMAT_EXE} -i ${app_src_glob}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
