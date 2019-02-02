file(GLOB app_src_glob *.cpp;*.cc;*.h;*.inl)

include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

set(app_src ${app_src_glob})
source_group("" FILES ${app_src})

set(all_src ${app_src} ${src_glew})

include_directories(SYSTEM
                    ../external/imgui/
                    )

# release date 2017-09-23 version duktape-2.2.0.tar.xz
include_directories(SYSTEM ../external/duktape/src)

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
SET(Gaf_CUSTOM_PREFIX gaf_)
GAF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
                      ../file_types/world.gaf
                      ../file_types/game.gaf
                      )
set(all_src ${all_src} ${PROTO_SRCS} ${PROTO_HDRS})

add_executable(engine WIN32 MACOSX_BUNDLE ${all_src})

target_link_libraries(engine
                      core
                      render
                      window
                      ${SDL2_LIBRARY}
                      )

MESSAGE(STATUS "SDL2 libraries are ${SDL2_LIBRARY}")
MESSAGE(STATUS "SDL2 main library is ${SDL2MAIN_LIBRARY}")

if(CMAKE_COMPILER_IS_GNUCC)
  set_property(TARGET engine APPEND_STRING PROPERTY COMPILE_FLAGS -Wall)
endif()

if(CLANG_TIDY_EXE)
  # -fix
  ADD_CUSTOM_TARGET(tidy-engine
                    COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} ${app_src_glob}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if(CLANG_FORMAT_EXE)
  # -fix
  ADD_CUSTOM_TARGET(format-engine
                    COMMAND ${CLANG_FORMAT_EXE} -i ${app_src_glob}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()


