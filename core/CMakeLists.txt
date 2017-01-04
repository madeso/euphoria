file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})

include_directories(../external/aheasing)
set(src_aheasing ../external/aheasing/aheasing/easing.c ../external/aheasing/aheasing/easing.h)
set(src ${src} ${src_aheasing})
source_group("AHEasing" FILES ${src_aheasing})

add_library(libcore STATIC ${src})

target_link_libraries(libcore
    ${PROTOBUF_LIBRARIES}
    )
