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

	if(MSVC12)
		set(ASSIMP_MSVC_VERSION "vc120")
	elseif(MSVC14)	
		set(ASSIMP_MSVC_VERSION "vc140")
	endif(MSVC12)

	message(STATUS "CMAKE assimp msvc version ${ASSIMP_MSVC_VERSION}")
	
	if(MSVC12 OR MSVC14)
	
		find_path(ASSIMP_LIBRARY_DIR
			NAMES
				assimp-${ASSIMP_MSVC_VERSION}-mt.lib
			HINTS
				${ASSIMP_ROOT_DIR}/lib${ASSIMP_ARCHITECTURE}
				${ASSIMP_ROOT_DIR}/lib
		)

		message(STATUS "Assimp library directory ${ASSIMP_LIBRARY_DIR}")

		# release should be assimp-vc140-mt.dll
		find_library(ASSIMP_LIBRARY_RELEASE				assimp-${ASSIMP_MSVC_VERSION}-mt.lib 			PATHS ${ASSIMP_LIBRARY_DIR})
		find_library(ASSIMP_LIBRARY_DEBUG				assimp-${ASSIMP_MSVC_VERSION}-mtd.lib			PATHS ${ASSIMP_LIBRARY_DIR})
		
		set(assimp_LIBRARIES
			optimized 	${ASSIMP_LIBRARY_RELEASE}
			debug		${ASSIMP_LIBRARY_DEBUG}
		)
	
		FUNCTION(ASSIMP_COPY_BINARIES TargetDirectory)
			ADD_CUSTOM_TARGET(AssimpCopyBinaries
				COMMAND ${CMAKE_COMMAND} -E copy ${ASSIMP_ROOT_DIR}/bin${ASSIMP_ARCHITECTURE}/assimp-${ASSIMP_MSVC_VERSION}-mtd.dll 	${TargetDirectory}/Debug/assimp-${ASSIMP_MSVC_VERSION}-mtd.dll
				COMMAND ${CMAKE_COMMAND} -E copy ${ASSIMP_ROOT_DIR}/bin${ASSIMP_ARCHITECTURE}/assimp-${ASSIMP_MSVC_VERSION}-mt.dll 		${TargetDirectory}/Release/assimp-${ASSIMP_MSVC_VERSION}-mt.dll
			COMMENT "Copying Assimp binaries to '${TargetDirectory}'"
			VERBATIM)
		ENDFUNCTION(ASSIMP_COPY_BINARIES)
	
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
		message(STATUS "Found asset importer library: ${assimp_LIBRARIES}")
	  endif (NOT assimp_FIND_QUIETLY)
	else (assimp_FOUND)
	  if (assimp_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find asset importer library")
	  endif (assimp_FIND_REQUIRED)
	endif (assimp_FOUND)
	
endif(WIN32)
