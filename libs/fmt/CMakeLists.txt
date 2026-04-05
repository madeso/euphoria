add_library(euph_fmt INTERFACE)
add_library(euph::fmt ALIAS euph_fmt)
target_include_directories(euph_fmt INTERFACE src)
target_link_libraries(euph_fmt INTERFACE external::fmt)
