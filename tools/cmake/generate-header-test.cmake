#
#
#   FUNCTION:
#     generate_header_test(_library_ INCLUDE_BASE _base_ FILES _files_)
#
#   Generate a project that includes each header file seperatly
#   Purpose: to test public and private include paths for a library
#
#   Usage:
#
#     add_library(my_library ${src})
#
#     generate_header_test(my_library
#         INCLUDE_BASE "${CMAKE_CURRENT_SOURCE_DIR}/src"
#         FILES ${src}
#     )
#
#
function(generate_header_test LIBRARY)
    set(options)
    set(oneValueArgs INCLUDE_BASE)
    set(multiValueArgs FILES)
    cmake_parse_arguments(PARSE_ARGV 1 ARG "${options}" "${oneValueArgs}" "${multiValueArgs}")
    set(target_name "${LIBRARY}_test_includes")

    set(src)

    foreach(input_file ${ARG_FILES})
        cmake_path(GET input_file EXTENSION LAST_ONLY input_ext)

        if("${input_ext}" STREQUAL ".h")
            cmake_path(ABSOLUTE_PATH input_file)
            cmake_path(RELATIVE_PATH input_file BASE_DIRECTORY "${ARG_INCLUDE_BASE}" OUTPUT_VARIABLE include_file)
            cmake_path(GET input_file FILENAME input_name)
            set(output_file "${CMAKE_CURRENT_BINARY_DIR}/include_test/test_${input_name}.cc")
            file(WRITE ${output_file} "#include \"${include_file}\"\n")
            # message(STATUS "  include file ${include_file}")
            # message(STATUS "  output file ${output_file}")
            set(src ${src} ${output_file})
        endif()
    endforeach()

    source_group("tests" FILES ${src})

    set(output_file "${CMAKE_CURRENT_BINARY_DIR}/include_test/main.cc")
    file(WRITE ${output_file} "int main(){return 0;}\n")
    source_group("" FILES ${output_file})
    set(src ${src} ${output_file})

    add_executable(${target_name} ${src})
    target_link_libraries(${target_name} PRIVATE project_options project_warnings ${LIBRARY})
    set_target_properties(${target_name} PROPERTIES FOLDER "Tests/Headers")
    # message(STATUS "relative include is ${ARG_INCLUDE_BASE}")
    # message(STATUS "source for ${target_name} is ${src}")
endfunction()
