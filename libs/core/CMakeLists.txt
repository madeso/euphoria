###############################################################################
# core project

set(core_src
    src/core/image.cc src/core/image.h
    src/core/loadedfont.cc src/core/loadedfont.h
    src/core/utf8.cc src/core/utf8.h

    src/core/pch.private.h
)

source_group("" FILES ${core_src})

set(src
    ${core_src}
)

add_library(core STATIC ${src})
target_precompile_headers(core
    PRIVATE src/core/pch.private.h
)

add_library(euph::core ALIAS core)
set_target_properties(core PROPERTIES FOLDER "Libraries")
target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(core
    PUBLIC
        euph::assert
        euph::base
        external::stb_truetype
        embed::embed
    PRIVATE
        euph::project_options
        euph::project_warnings
)