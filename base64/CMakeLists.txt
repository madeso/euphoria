file(GLOB app_src *.cc;*.h;*.inl)
set(src ${app_src})
source_group("" FILES ${app_src})

add_executable(base64 ${src})

SET_PROPERTY(TARGET base64 PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)
TARGET_LINK_LIBRARIES(base64 core)
