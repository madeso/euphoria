find_package(wxWidgets COMPONENTS core base)
include( "${wxWidgets_USE_FILE}" )

file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})

add_executable(scimed WIN32 MACOSX_BUNDLE ${src})
set_target_properties(scimed
                      PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/dist"
                      DEBUG_OUTPUT_NAME "scimed-debug"
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/dist"
                      RELEASE_OUTPUT_NAME "scimed"
                      )

target_link_libraries(scimed
                      ${wxWidgets_LIBRARIES}
                      ${PROTOBUF_LIBRARIES}
                      )
