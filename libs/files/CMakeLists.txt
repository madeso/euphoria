###############################################################################
# files project

set(app_src
    src/files/font.cc src/files/font.h
    src/files/game.cc src/files/game.h
    src/files/gui.cc src/files/gui.h
    src/files/materialshader.cc src/files/materialshader.h
    src/files/mesh.cc src/files/mesh.h
    src/files/scalingsprite.cc src/files/scalingsprite.h
    src/files/world.cc src/files/world.h
    src/files/enum.cc src/files/enum.h
)

source_group("" FILES ${app_src})

set(src
    ${app_src}
)

add_library(files STATIC ${src})
add_library(euph::files ALIAS files)
set_target_properties(files PROPERTIES FOLDER "Libraries")
target_include_directories(files
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(files
    PUBLIC
        euph::log
        euph::assert
        euph::fmt
        euph::io
        jsonh::jsonh
    PRIVATE
        euph::project_options
        euph::project_warnings
)

