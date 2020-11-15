###############################################################################
# duk project

set(src
    src/duk/config.h
    src/duk/types.h
    src/duk/bind.cc src/duk/bind.h
    src/duk/bindclass.cc src/duk/bindclass.h
    src/duk/bindobject.cc src/duk/bindobject.h
    src/duk/context.cc src/duk/context.h
    src/duk/duk.cc src/duk/duk.h
    src/duk/function.cc src/duk/function.h
    src/duk/functionreference.cc src/duk/functionreference.h
    src/duk/genericfunction.cc src/duk/genericfunction.h
    src/duk/objectreference.cc src/duk/objectreference.h
    src/duk/optional.cc src/duk/optional.h
    src/duk/pushvar.cc src/duk/pushvar.h
    src/duk/referencestorage.cc src/duk/referencestorage.h
    src/duk/registeredclass.cc src/duk/registeredclass.h
    src/duk/stackparser.cc src/duk/stackparser.h
    src/duk/storedreference.cc src/duk/storedreference.h
    src/duk/util.cc src/duk/util.h
)
source_group("" FILES ${src})

add_library(duk STATIC ${src})
target_include_directories(duk
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(duk
    core
    duktape
)


###############################################################################
# test project

set(test_src
    src/duk/duk.test.cc
)
set(test_main_src test/main.cc)
source_group("" FILES ${test_src})
source_group("main" FILES ${test_main_src})
add_executable(duk_test ${test_src} ${test_main_src})
target_link_libraries(duk_test PUBLIC duk tests)

