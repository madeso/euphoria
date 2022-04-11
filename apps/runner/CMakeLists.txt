###############################################################################
# runner project

set(src
    src/runner/cameradata.cc src/runner/cameradata.h
    src/runner/components.cc src/runner/components.h
    src/runner/dukintegration.cc src/runner/dukintegration.h
    src/runner/dukmathbindings.cc src/runner/dukmathbindings.h
    src/runner/dukprint.cc src/runner/dukprint.h
    src/runner/dukregistry.cc src/runner/dukregistry.h
    src/runner/input.cc src/runner/input.h
    src/runner/loadworld.cc src/runner/loadworld.h
    src/runner/objectemplate.cc src/runner/objectemplate.h
    src/runner/systems.cc src/runner/systems.h
    src/runner/ecs.systems.cc src/runner/ecs.systems.h
    src/runner/main.cc
)
source_group("" FILES ${src})

add_executable(runner MACOSX_BUNDLE ${src})
set_win32_defines(runner)
set_target_properties(runner PROPERTIES FOLDER "Applications")
target_include_directories(runner
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(runner
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::core
        euph::render
        euph::window
        euph::file_types
        external::lua
        external::imgui
        external::sdl2
)

