file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})

# set(src ${src} ${GMOCK_SOURCES})
# source_group("gmock" FILES ${GMOCK_SOURCES})
# include_directories(${GMOCK_INCLUDE_DIRS})
# add_definitions(-DGTEST_USE_OWN_TR1_TUPLE=1)

include_directories(SYSTEM ../external/catch/)

add_executable(tests ${src})

enable_testing()
add_test(tests tests)

TARGET_LINK_LIBRARIES(tests core duk minsynth ${CMAKE_THREAD_LIBS_INIT})
