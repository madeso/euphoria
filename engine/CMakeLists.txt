set(app_src
    cameradata.cc cameradata.h
    components.cc components.h
    dukintegration.cc dukintegration.h
    dukmathbindings.cc dukmathbindings.h
    dukprint.cc dukprint.h
    dukregistry.cc dukregistry.h
    input.cc input.h
    loadworld.cc loadworld.h
    main.cc
    objectemplate.cc objectemplate.h
    systems.cc systems.h
)
source_group("" FILES ${app_src})

include_directories(SYSTEM ${SDL2_INCLUDE_DIR})


set(all_src ${app_src} ${src_glew})

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
                      duk
                      ${SDL2_LIBRARY}
                      )
