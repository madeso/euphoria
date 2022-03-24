# Set to FALSE to not generate per-target graphs foo.dot.<target>.
set(GRAPHVIZ_GENERATE_PER_TARGET FALSE)

# Set to FALSE to not generate depender graphs foo.dot.<target>.dependers.
set(GRAPHVIZ_GENERATE_DEPENDERS FALSE)

# A list of regular expressions for names of targets to exclude from the generated graphs.
set(GRAPHVIZ_IGNORE_TARGETS
    "^examples-"
    "^libgaf_"
    "^gaf"
    "_test$"
    "^tests$"
    "^core_test_includes$"
    "^catch$"
    "^project_warnings$"
    "^project_options$"
)

# Set to FALSE to exclude unknown libraries from the generated graphs.
set(GRAPHVIZ_UNKNOWN_LIBS FALSE)
# unknown seems to be various .so and compiler flags

set(GRAPHVIZ_EXTERNAL_LIBS FALSE)
set(GRAPHVIZ_INTERFACE_LIBS FALSE)
