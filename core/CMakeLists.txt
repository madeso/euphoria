file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})

find_package(AssImp REQUIRED)

message(STATUS "Assimp includes at ${assimp_INCLUDE_DIRS}")
message(STATUS "Assimp libraries at ${assimp_LIBRARIES}")

include_directories(SYSTEM ${assimp_INCLUDE_DIRS})

# todo: move image loading to a io library instead
include_directories(SYSTEM ../external/stb)

include_directories(../external/aheasing)
set(src_aheasing
    ../external/aheasing/aheasing/easing.c
    ../external/aheasing/aheasing/easing.h
    )

include_directories(../external/pbjson/src/)
set(pbjson_src
    ../external/pbjson/src/pbjson.cpp
    ../external/pbjson/src/pbjson.hpp
    ../external/pbjson/src/bin2ascii.h
    )

include_directories(../external/font8x8/)

set(src ${src} ${src_aheasing} ${pbjson_src})
source_group("AHEasing" FILES ${src_aheasing})

add_library(core STATIC ${src})

target_link_libraries(core
    ${PROTOBUF_LIBRARIES}
    ${assimp_LIBRARIES}
    )
