file(GLOB app_src_glob *.cpp;*.cc;*.h;*.inl)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

include_directories(../external/glm/include)

include_directories(../external/stb)

set(app_src ${app_src_glob})
source_group("" FILES ${app_src})

# generate shader headers
set(src_shaders
    ${CMAKE_BINARY_DIR}/generated_shaders.h
    ${CMAKE_BINARY_DIR}/generated_shaders.cc
    )
source_group("generated shader" FILES ${src_shaders})
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/generated_shaders.h ${CMAKE_BINARY_DIR}/generated_shaders.cc
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/files_to_functions.py ${CMAKE_BINARY_DIR}/generated_shaders.h ${CMAKE_BINARY_DIR}/generated_shaders.cc shader_source_
                   shaders/sprite.vert shaders/sprite.frag
    DEPENDS files_to_functions.py
                   shaders/sprite.vert shaders/sprite.frag
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating shaders."
)


set(all_src ${app_src} ${src_shaders})

add_executable(spacetyper WIN32 MACOSX_BUNDLE ${all_src})
set_target_properties(spacetyper
                      PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/dist"
                      DEBUG_OUTPUT_NAME "spacetyper-debug"
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/dist"
                      RELEASE_OUTPUT_NAME "spacetyper"
                      )

target_link_libraries(spacetyper
                      ${OPENGL_LIBRARIES}
                      ${SDL2_LIBRARY}
                      )

