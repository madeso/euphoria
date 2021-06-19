###############################################################################
# file_types project

SET(Gaf_CUSTOM_PREFIX gaf_)
GAF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
    src/file_types/enum.gaf
    src/file_types/mesh.gaf
    src/file_types/chatbot.gaf
    src/file_types/scalingsprite.gaf
    src/file_types/materialshader.gaf
    src/file_types/font.gaf
    src/file_types/gui.gaf
    src/file_types/world.gaf
    src/file_types/game.gaf
)
add_library(file_types STATIC
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)
set_target_properties(file_types PROPERTIES FOLDER "Libraries")
target_include_directories(file_types
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(file_types
    PRIVATE
        rapidjson
)

