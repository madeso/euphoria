file(GLOB app_src *.cc;*.h;*.inl)
file(GLOB app_only_src *.cc)
set(src ${app_src})
source_group("" FILES ${app_src})
set(src ${app_src})



find_package(assimp REQUIRED)
# message(STATUS "Assimp includes at ${assimp_INCLUDE_DIRS}")
# message(STATUS "Assimp libraries at ${assimp_LIBRARIES}")
include_directories(SYSTEM ${assimp_INCLUDE_DIRS})



# todo: move image loading to a io library instead
include_directories(SYSTEM ../external/stb)



include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ../proto/enum.proto)
set(src ${src} ${PROTO_SRCS} ${PROTO_HDRS})



include_directories(../external/aheasing)
set(src_aheasing
    ../external/aheasing/aheasing/easing.c
    ../external/aheasing/aheasing/easing.h
    )
set(src ${src} ${src_aheasing})
source_group("AHEasing" FILES ${src_aheasing})




include_directories(../external/pbjson/src/)
set(pbjson_src
    ../external/pbjson/src/pbjson.cpp
    ../external/pbjson/src/pbjson.hpp
    ../external/pbjson/src/bin2ascii.h
    )
set(src ${src} ${pbjson_src})
source_group("pbjson" FILES ${pbjson_src})



include_directories(../external/font8x8/)

add_library(core STATIC ${src})
target_link_libraries(core
    ${PROTOBUF_LIBRARIES}
    ${assimp_LIBRARIES}
    )

if(CLANG_TIDY_EXE)
  # -fix
  ADD_CUSTOM_TARGET(tidy-core
                    COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} ${app_only_src}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

