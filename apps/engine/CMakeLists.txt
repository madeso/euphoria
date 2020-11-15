set(app_src
    src/engine/cameradata.cc src/engine/cameradata.h
    src/engine/components.cc src/engine/components.h
    src/engine/dukintegration.cc src/engine/dukintegration.h
    src/engine/dukmathbindings.cc src/engine/dukmathbindings.h
    src/engine/dukprint.cc src/engine/dukprint.h
    src/engine/dukregistry.cc src/engine/dukregistry.h
    src/engine/input.cc src/engine/input.h
    src/engine/loadworld.cc src/engine/loadworld.h
    src/engine/objectemplate.cc src/engine/objectemplate.h
    src/engine/systems.cc src/engine/systems.h
    src/engine/main.cc
)
source_group("" FILES ${app_src})
set(src ${app_src})

add_executable(engine WIN32 MACOSX_BUNDLE ${src})
target_include_directories(engine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(engine
    PRIVATE
    core
    render
    window
    duk
    imgui
    sdl2
    file_types
)

