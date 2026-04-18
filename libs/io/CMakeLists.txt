###############################################################################
# io project

set(io_src
    src/eu/io/mesh.cc src/eu/io/mesh.h
    src/eu/io/file.cc src/eu/io/file.h

    src/eu/io/pch.private.h
)

source_group("" FILES ${io_src})
cmake_source_group()

set(src
    ${io_src}
)

add_library(io STATIC ${src})
target_precompile_headers(io PRIVATE src/eu/io/pch.private.h)

add_library(euph::io ALIAS io)
set_target_properties(io PROPERTIES FOLDER "Libraries")
target_include_directories(io
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(io
    PUBLIC
        euph::assert
        euph::base
        euph::core
        external::cgltf
    PRIVATE
        euph::project_options
        euph::project_warnings
)
