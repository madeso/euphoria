###############################################################################
# runner project
set(src
    src/eu/runner/main.cc
    src/eu/runner/pch.private.h
)
source_group("" FILES ${src})
cmake_source_group()

# add_executable(runner WIN32 MACOSX_BUNDLE ${src})
add_executable(runner ${src})
target_precompile_headers(runner PRIVATE src/eu/runner/pch.private.h)
set_target_properties(runner PROPERTIES FOLDER "Applications")
target_include_directories(runner
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(runner
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::log
        euph::base
        euph::io
        euph::render
        euph::kdl
        external::sdl2
        external::opengl
        external::glad
)

if(EU_DEBUG_RUNNER)
    target_link_libraries(runner
        PRIVATE
            euph::imgui
            external::dear_imgui
    )
endif()