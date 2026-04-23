set(kdl_src
    src/eu/kdl/kdl.cc
    src/eu/kdl/kdl.h
)
cmake_source_group()
source_group("" FILES $(kdl_src))
add_library(kdl STATIC
    ${kdl_src}
)

set_target_properties(kdl PROPERTIES FOLDER "Libraries")
target_include_directories(kdl SYSTEM
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(kdl
    PUBLIC
        external::ckdl
    PRIVATE
        euph::project_options
        euph::project_options
)

add_library(euph::kdl ALIAS kdl)
