###############################################################################
# musikmaskin project

set(src
    src/musikmaskin/main.cc
)
source_group("" FILES ${src})

# todo(Gustav): bring back rtmidi as a cmake build but properly this time
set(RTMIDI_ROOT ../../external/rtmidi)
include_directories(SYSTEM ${RTMIDI_ROOT})
set(rtmidi_src
    ${RTMIDI_ROOT}/RtMidi.cpp
    ${RTMIDI_ROOT}/RtMidi.h
)
set(src ${src} ${rtmidi_src})
source_group("rtmidi" FILES ${rtmidi_src})


add_executable(musikmaskin WIN32 MACOSX_BUNDLE ${src})
set_target_properties(musikmaskin PROPERTIES FOLDER "Applications")
target_include_directories(musikmaskin
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(musikmaskin
    PRIVATE
        euph::project_options
        euph::project_warnings
        euph::core
        euph::minsynth
        euph::render
        euph::window
        euph::fmt
        external::imgui
        external::sdl2
)

