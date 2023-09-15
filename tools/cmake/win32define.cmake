
function(set_win32_defines target)
    get_property(is_executable
        TARGET ${target}
        PROPERTY WIN32_EXECUTABLE
        SET
    )
    if(is_executable)
        # it is a win32 executable, so don't force console
        target_compile_definitions(
            ${target} PRIVATE
            EU_LOG_FORCE_CONSOLE=false
        )
        # message(STATUS "${target} is a win32 executable")
    else()
        # it is a console executable, so we need to force console now
        target_compile_definitions(
            ${target} PRIVATE
            EU_LOG_FORCE_CONSOLE=true
        )
        # message(STATUS "${target} is a console executable")
    endif()
endfunction()
