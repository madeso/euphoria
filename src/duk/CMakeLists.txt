file(GLOB app_src *.cc;*.h;*.inl)
file(GLOB app_only_src *.cc)
set(src ${app_src})
source_group("" FILES ${app_src})
set(src ${app_src})


# release date 2017-09-23 version duktape-2.2.0.tar.xz
include_directories(SYSTEM ../../external/duktape)
set(src ${src} ../../external/duktape/duktape/duktape.c)



add_library(duk STATIC ${src})
target_link_libraries(duk
                      core
                      )
