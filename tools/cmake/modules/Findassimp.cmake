if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ASSIMP_ARCHITECTURE "64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ASSIMP_ARCHITECTURE "32")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    
if(WIN32)
    set(ASSIMP_ROOT_DIR CACHE PATH "ASSIMP root directory")
    message(STATUS "CMAKE Finding assimp ${ASSIMP_ROOT_DIR}")

    # Find path of each library
    find_path(assimp_INCLUDE_DIRS
        NAMES
            assimp/anim.h
        HINTS
            ${ASSIMP_ROOT_DIR}/include
    )

    # code block stolen from assimp cmake file
    if( MSVC70 OR MSVC71 )
      set(MSVC_PREFIX "vc70")
    elseif( MSVC80 )
      set(MSVC_PREFIX "vc80")
    elseif( MSVC90 )
      set(MSVC_PREFIX "vc90")
    elseif( MSVC10 )
      set(MSVC_PREFIX "vc100")
    elseif( MSVC11 )
      set(MSVC_PREFIX "vc110")
    elseif( MSVC12 )
      set(MSVC_PREFIX "vc120")
    elseif( MSVC_VERSION LESS 1910)
      set(MSVC_PREFIX "vc140")
    elseif( MSVC_VERSION LESS 1920)
      set(MSVC_PREFIX "vc141")
    elseif( MSVC_VERSION LESS 1930)
      set(MSVC_PREFIX "vc142")
    elseif( MSVC_VERSION LESS 1940)
      set(MSVC_PREFIX "vc143")
    else()
      MESSAGE(WARNING "unknown msvc version ${MSVC_VERSION}")
      set(MSVC_PREFIX "vc150")
    endif()

    set(ASSIMP_MSVC_VERSION "${MSVC_PREFIX}")

    message(STATUS "CMAKE assimp msvc version ${ASSIMP_MSVC_VERSION}")
    
    find_path(ASSIMP_LIBRARY_DIR
        NAMES
            assimp-${ASSIMP_MSVC_VERSION}-mt.lib
        HINTS
            ${ASSIMP_ROOT_DIR}/lib${ASSIMP_ARCHITECTURE}
            ${ASSIMP_ROOT_DIR}/lib
    )

    message(STATUS "Assimp library directory ${ASSIMP_LIBRARY_DIR}")

    # release should be assimp-vc140-mt.dll
    find_library(ASSIMP_LIBRARY_RELEASE assimp-${ASSIMP_MSVC_VERSION}-mt.lib  PATHS ${ASSIMP_LIBRARY_DIR})
    find_library(ASSIMP_LIBRARY_DEBUG   assimp-${ASSIMP_MSVC_VERSION}-mtd.lib PATHS ${ASSIMP_LIBRARY_DIR})

    find_library(ASSIMP_IRRXML_LIBRARY_RELEASE IrrXML.lib  PATHS ${ASSIMP_LIBRARY_DIR})
    find_library(ASSIMP_IRRXML_LIBRARY_DEBUG   IrrXMLd.lib PATHS ${ASSIMP_LIBRARY_DIR})

    find_library(ASSIMP_ZLIB_LIBRARY_RELEASE zlibstatic.lib  PATHS ${ASSIMP_LIBRARY_DIR})
    find_library(ASSIMP_ZLIB_LIBRARY_DEBUG   zlibstaticd.lib PATHS ${ASSIMP_LIBRARY_DIR})

    set(assimp_lib_release ${ASSIMP_LIBRARY_RELEASE})
    set(assimp_lib_debug ${ASSIMP_LIBRARY_DEBUG})
    
    if(ASSIMP_IRRXML_LIBRARY_RELEASE AND ASSIMP_IRRXML_LIBRARY_DEBUG)
        set(assimp_lib_debug   ${assimp_lib_debug}   ${ASSIMP_IRRXML_LIBRARY_DEBUG})
        set(assimp_lib_release ${assimp_lib_release} ${ASSIMP_IRRXML_LIBRARY_RELEASE})
    endif()
    if(ASSIMP_ZLIB_LIBRARY_RELEASE AND ASSIMP_ZLIB_LIBRARY_DEBUG)
        set(assimp_lib_debug   ${assimp_lib_debug}   ${ASSIMP_ZLIB_LIBRARY_RELEASE})
        set(assimp_lib_release ${assimp_lib_release} ${ASSIMP_ZLIB_LIBRARY_DEBUG})
    endif()

    if(ASSIMP_LIBRARY_DEBUG)
        set(assimp_LIBRARIES
            optimized
                ${assimp_lib_release}
            debug
                ${assimp_lib_debug}
        )
    else()
        set(assimp_LIBRARIES
            ${assimp_lib_release}
        )
    endif()
else(WIN32)

    find_path(
        assimp_INCLUDE_DIRS
        NAMES assimp/postprocess.h assimp/scene.h assimp/version.h assimp/config.h assimp/cimport.h
        PATHS /usr/local/include/
    )

    find_library(
        assimp_LIBRARIES
        NAMES assimp
        PATHS /usr/local/lib/
    )

    if (assimp_INCLUDE_DIRS AND assimp_LIBRARIES)
        SET(assimp_FOUND TRUE)
    ENDIF (assimp_INCLUDE_DIRS AND assimp_LIBRARIES)

    if (assimp_FOUND)
        if (NOT assimp_FIND_QUIETLY)
            # message(STATUS "Found asset importer library: ${assimp_LIBRARIES}")
        endif (NOT assimp_FIND_QUIETLY)
    else (assimp_FOUND)
        if (assimp_FIND_REQUIRED)
            message(FATAL_ERROR "Could not find asset importer library")
        endif (assimp_FIND_REQUIRED)
    endif (assimp_FOUND)
endif(WIN32)
