###############################################################################
# imgui_extra project

set(imgui_src
    src/imgui_extra/scurve.cc src/imgui_extra/scurve.h
    src/imgui_extra/ui.cc src/imgui_extra/ui.h

    src/imgui_extra/pch.private.h
)

source_group("" FILES ${imgui_src})

set(src
    ${imgui_src}
)

add_library(imgui_extra STATIC ${src})
target_precompile_headers(imgui_extra PRIVATE src/imgui_extra/pch.private.h)

add_library(euph::imgui_extra ALIAS imgui_extra)
set_target_properties(imgui_extra PROPERTIES FOLDER "Libraries")
target_include_directories(imgui_extra
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(imgui_extra
    PUBLIC
        euph::assert
        euph::base
        euph::core
        euph::render
        external::dear_imgui
        embed::embed
    PRIVATE
        euph::project_options
        euph::project_warnings
)

