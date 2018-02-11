file(GLOB app_src_glob *.cpp;*.cc;*.h;*.inl)

include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

set(app_src ${app_src_glob})
source_group("" FILES ${app_src})

set(all_src ${app_src} ${src_glew})

include_directories(SYSTEM
                    ../external/imgui/
                    )

include_directories(../external/entt/src)

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
                      ../proto/world.proto
                      )
set(all_src ${all_src} ${PROTO_SRCS} ${PROTO_HDRS})

add_executable(engine WIN32 MACOSX_BUNDLE ${all_src})

target_link_libraries(engine
                      core
                      render
                      window
                      ${SDL2_LIBRARY}
                      ${PROTOBUF_LIBRARIES}
                      )

MESSAGE(STATUS "SDL2 libraries are ${SDL2_LIBRARY}")
MESSAGE(STATUS "SDL2 main library is ${SDL2MAIN_LIBRARY}")

if(CMAKE_COMPILER_IS_GNUCC)
  set_property(TARGET engine APPEND_STRING PROPERTY COMPILE_FLAGS -Wall)
endif()

