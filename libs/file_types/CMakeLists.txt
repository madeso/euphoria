###############################################################################
# file_types project

set(all_gafs
    src/file_types/enum.gaf
    src/file_types/mesh.gaf
    src/file_types/chatbot.gaf
    src/file_types/scalingsprite.gaf
    src/file_types/materialshader.gaf
    src/file_types/font.gaf
    src/file_types/gui.gaf
    src/file_types/world.gaf
    src/file_types/game.gaf
    src/file_types/tracery.gaf
)
source_group("" FILES ${all_gafs})

set(generated_files)


GAF_GENERATE_CPP(
    SOURCES generated_sources
    HEADERS generated_headers
    FILES
        ${all_gafs}
)
set(generated_files
    ${generated_files}
    ${generated_sources}
    ${generated_headers}
)


GAF_GENERATE_CPP(
    PUGIXML
    SOURCES generated_sources
    HEADERS generated_headers
    FILES
        ${all_gafs}
)
set(generated_files
    ${generated_files}
    ${generated_sources}
    ${generated_headers}
)
source_group("generated" FILES ${generated_files})


add_library(file_types STATIC ${generated_files} ${all_gafs})
set_target_properties(file_types PROPERTIES FOLDER "Libraries")
target_include_directories(file_types
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(file_types
    PUBLIC
        pugixml
        libgaf_xml
    PRIVATE
        project_options
        project_warnings
)

