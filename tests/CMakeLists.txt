file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})

# todo: move to core and make a library
set(src ${src}
    "../spacetyper/numeric.cc"
    "../spacetyper/axisangle.cc"
    "../spacetyper/angle.cc"
    )


set(src ${src} ${GMOCK_SOURCES})
source_group("gmock" FILES ${GMOCK_SOURCES})
include_directories(${GMOCK_INCLUDE_DIRS})
# add_definitions(-DGTEST_USE_OWN_TR1_TUPLE=1)



add_executable(tests ${src})

enable_testing()
add_test(tests tests)

SET_PROPERTY(TARGET tests PROPERTY CXX_STANDARD 11)
SET_PROPERTY(TARGET tests PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)
TARGET_LINK_LIBRARIES(tests ${CMAKE_THREAD_LIBS_INIT})
