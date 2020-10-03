set(app_src
    config.h types.h

    bind.cc bind.h
    bindclass.cc bindclass.h
    bindobject.cc bindobject.h
    context.cc context.h
    duk.cc duk.h
    function.cc function.h
    functionreference.cc functionreference.h
    genericfunction.cc genericfunction.h
    objectreference.cc objectreference.h
    optional.cc optional.h
    pushvar.cc pushvar.h
    referencestorage.cc referencestorage.h
    registeredclass.cc registeredclass.h
    stackparser.cc stackparser.h
    storedreference.cc storedreference.h
    util.cc util.h
)
source_group("" FILES ${app_src})
set(src ${app_src})

add_library(duk STATIC ${src})
target_link_libraries(duk
    core
    duktape
)

