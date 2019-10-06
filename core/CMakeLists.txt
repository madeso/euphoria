file(GLOB app_src *.cc;*.h;*.inl)
file(GLOB app_only_src *.cc)
set(src ${app_src})
source_group("" FILES ${app_src})
set(src ${app_src})


MESSAGE(STATUS "Looking for assimp")
find_package(assimp REQUIRED)
message(STATUS "Assimp includes at ${assimp_INCLUDE_DIRS}")
message(STATUS "Assimp libraries at ${assimp_LIBRARIES}")
include_directories(SYSTEM ${assimp_INCLUDE_DIRS})



# todo: move image loading to a io library instead
include_directories(SYSTEM ../external/stb)



# release date 2017-09-23 version duktape-2.2.0.tar.xz
include_directories(SYSTEM ../external/duktape)
set(src ${src} ../external/duktape/duktape/duktape.c)



include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
SET(Gaf_CUSTOM_PREFIX gaf_)
GAF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
                      ../file_types/enum.gaf
                      ../file_types/mesh.gaf
                      ../file_types/chatbot.gaf
                      )
set(src ${src} ${PROTO_SRCS} ${PROTO_HDRS})



include_directories(../external/aheasing)
set(src_aheasing
    ../external/aheasing/aheasing/easing.c
    ../external/aheasing/aheasing/easing.h
    )
set(src ${src} ${src_aheasing})
source_group("AHEasing" FILES ${src_aheasing})


include_directories(../external/font8x8/)

add_library(core STATIC ${src})
target_link_libraries(core
    ${assimp_LIBRARIES}
    )
