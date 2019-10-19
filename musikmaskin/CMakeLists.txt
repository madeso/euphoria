file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})

include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

add_executable(musikmaskin WIN32 MACOSX_BUNDLE ${app_src})
target_link_libraries(musikmaskin
                      core
                      minsynth
                      render
                      window
                      ${SDL2_LIBRARY}
                      ${CMAKE_DL_LIBS}
                      )

if(${BUILD_MIDI_SUPPORT})
target_link_libraries(musikmaskin rtmidi)
endif(${BUILD_MIDI_SUPPORT})
