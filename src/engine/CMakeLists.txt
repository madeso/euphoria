set(app_src
    cameradata.cc cameradata.h
    components.cc components.h
    dukintegration.cc dukintegration.h
    dukmathbindings.cc dukmathbindings.h
    dukprint.cc dukprint.h
    dukregistry.cc dukregistry.h
    input.cc input.h
    loadworld.cc loadworld.h
    objectemplate.cc objectemplate.h
    systems.cc systems.h
    main.cc
)
source_group("" FILES ${app_src})
set(src ${app_src})

add_executable(engine WIN32 MACOSX_BUNDLE ${src})

target_link_libraries(engine
    PRIVATE
    core
    render
    window
    duk
    sdl2
    file_types
)
