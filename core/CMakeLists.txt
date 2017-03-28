file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})

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

set(src ${src} ${src_aheasing} ${pbjson_src})
source_group("AHEasing" FILES ${src_aheasing})

add_library(core STATIC ${src})

target_link_libraries(core
    ${PROTOBUF_LIBRARIES}
    )
