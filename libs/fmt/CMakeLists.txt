set(app_src
    src/fmt.cc src/pch.public.h
)
source_group("" FILES ${app_src})

set(src
    ${app_src}
)

add_library(euph_fmt STATIC ${src})
add_library(euph::fmt ALIAS euph_fmt)
set_target_properties(euph_fmt PROPERTIES FOLDER "Libraries")
# target_include_directories(euph_fmt
#     PUBLIC
#         ${CMAKE_CURRENT_SOURCE_DIR}/src
# )
target_link_libraries(euph_fmt
    PUBLIC
        external::fmt
    PRIVATE
        euph::project_options
        euph::project_warnings
)
target_precompile_headers(euph_fmt
    PUBLIC src/pch.public.h
)
