set(app_src main.cc)
set(src ${app_src})
source_group("" FILES ${app_src})

# wip proper rtmidi integration
set(RTMIDI_ROOT ../../external/rtmidi)
include_directories(SYSTEM ${RTMIDI_ROOT})
set(rtmidi_src
    ${RTMIDI_ROOT}/RtMidi.cpp
    ${RTMIDI_ROOT}/RtMidi.h
)
set(src ${src} ${rtmidi_src})
source_group("rtmidi" FILES ${rtmidi_src})

include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

add_executable(musikmaskin WIN32 MACOSX_BUNDLE ${src})
target_link_libraries(musikmaskin
                      core
                      minsynth
                      render
                      window
                      ${SDL2_LIBRARY}
                      ${CMAKE_DL_LIBS}
                      )

