file(GLOB app_src *.cc;*.h;*.inl)
file(GLOB app_only_src *.cc)
set(src ${app_src})
source_group("" FILES ${app_src})
set(src ${app_src})


MESSAGE(STATUS "Looking for assimp")
find_package(assimp REQUIRED)
message(STATUS "Assimp includes at ${assimp_INCLUDE_DIRS}")
message(STATUS "Assimp libraries at ${assimp_LIBRARIES}")
include_directories(SYSTEM ${assimp_INCLUDE_DIRS})



# todo: move image loading to a io library instead
include_directories(SYSTEM ../external/stb)

set(src ${src}
    ../external/lua/src/lapi.c
    ../external/lua/src/lauxlib.c
    ../external/lua/src/lbaselib.c
    ../external/lua/src/lbitlib.c
    ../external/lua/src/lcode.c
    ../external/lua/src/lcorolib.c
    ../external/lua/src/lctype.c
    ../external/lua/src/ldblib.c
    ../external/lua/src/ldebug.c
    ../external/lua/src/ldo.c
    ../external/lua/src/ldump.c
    ../external/lua/src/lfunc.c
    ../external/lua/src/lgc.c
    ../external/lua/src/linit.c
    ../external/lua/src/liolib.c
    ../external/lua/src/llex.c
    ../external/lua/src/lmathlib.c
    ../external/lua/src/lmem.c
    ../external/lua/src/loadlib.c
    ../external/lua/src/lobject.c
    ../external/lua/src/lopcodes.c
    ../external/lua/src/loslib.c
    ../external/lua/src/lparser.c
    ../external/lua/src/lstate.c
    ../external/lua/src/lstring.c
    ../external/lua/src/lstrlib.c
    ../external/lua/src/ltable.c
    ../external/lua/src/ltablib.c
    ../external/lua/src/ltm.c
    ../external/lua/src/lundump.c
    ../external/lua/src/lutf8lib.c
    ../external/lua/src/lvm.c
    ../external/lua/src/lzio.c
    )



include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})
SET(Gaf_CUSTOM_PREFIX gaf_)
GAF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
                      ../file_types/enum.gaf
                      ../file_types/mesh.gaf
                      ../file_types/chatbot.gaf
                      )
set(src ${src} ${PROTO_SRCS} ${PROTO_HDRS})



include_directories(../external/aheasing)
set(src_aheasing
    ../external/aheasing/aheasing/easing.c
    ../external/aheasing/aheasing/easing.h
    )
set(src ${src} ${src_aheasing})
source_group("AHEasing" FILES ${src_aheasing})


include_directories(../external/font8x8/)

add_library(core STATIC ${src})
target_link_libraries(core
    ${assimp_LIBRARIES}
    )

if(CLANG_TIDY_EXE)
  # -fix
  ADD_CUSTOM_TARGET(tidy-core
                    COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} ${app_only_src}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if(CLANG_FORMAT_EXE)
  # -fix
  ADD_CUSTOM_TARGET(format-core
                    COMMAND ${CLANG_FORMAT_EXE} -i ${app_src}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

