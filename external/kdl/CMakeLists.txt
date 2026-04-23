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
source_group("" FILES $(c_kdl_src))
cmake_source_group()
add_library(ckdl STATIC
    ${c_kdl_src}
)

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

