set(c_kdl_src
    src/bigint.c src/bigint.h
    src/compat.c src/compat.h
    src/emitter.c
    src/grammar.h
    src/parser.c
    src/str.c src/str.h
    src/tokenizer.c
    src/utf8.c src/utf8.h
)
add_library(ckdl STATIC
    ${c_kdl_src}
)
source_group("" FILEs $(c_kdl_src))
cmake_source_group()

set_target_properties(ckdl PROPERTIES FOLDER "Dependencies")
target_include_directories(ckdl SYSTEM
    PUBLIC
        include
)
target_compile_definitions(ckdl
    PUBLIC
        -DKDL_STATIC_LIB=1
)
add_library(external::ckdl ALIAS ckdl)



set(kdlpp_src
    cpp/include/kdlpp.h
    cpp/src/kdlpp.cpp
)
add_library(kdlpp STATIC
    ${kdlpp_src}
)
source_group("" FILEs $(kdlpp_src))
cmake_source_group()

set_target_properties(kdlpp PROPERTIES FOLDER "Dependencies")
target_include_directories(kdlpp SYSTEM
    PUBLIC
        cpp/include
)
target_compile_definitions(kdlpp
    PUBLIC
        -DKDLPP_STATIC_LIB=1
)
target_link_libraries(kdlpp
    PUBLIC
        external::ckdl
        euph::project_options
)
add_library(external::kdlpp ALIAS kdlpp)

