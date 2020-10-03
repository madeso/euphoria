SET(Gaf_CUSTOM_PREFIX gaf_)
GAF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
    enum.gaf
    mesh.gaf
    chatbot.gaf
    scalingsprite.gaf
    materialshader.gaf
    font.gaf
    gui.gaf
    world.gaf
    game.gaf
)
add_library(file_types STATIC
    ${PROTO_SRCS} ${PROTO_HDRS}
)
target_include_directories(file_types
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)

