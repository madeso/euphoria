file(GLOB src *.cc;*.h;*.inl)
source_group("" FILES ${src})

add_library(minsynth STATIC ${src})
target_link_libraries(minsynth
  core
  )
