# Install NcEngine runtime dependencies. SHARED_LIB_DESTINATION should be relative to CMAKE_INSTALL_PREFIX.
function (nc_install_runtime_dependencies SHARED_LIB_DESTINATION)
    if(WIN32)
        install(TARGETS ${DILIGENT_RUNTIME_LIBRARIES}
                RUNTIME DESTINATION ${SHARED_LIB_DESTINATION}
        )

        if(${NC_PROFILING_ENABLED})
            install(FILES       $<TARGET_FILE:OptickCore>
                    DESTINATION ${SHARED_LIB_DESTINATION}
            )
        endif()
    elseif(UNIX AND NOT APPLE)
        install(TARGETS ${DILIGENT_RUNTIME_LIBRARIES}
                LIBRARY DESTINATION ${SHARED_LIB_DESTINATION}
        )
    endif()
endfunction()

# Append NcEngine runtime dependency paths to a target's rpath.
# Options:
#   [LIBRARY_INSTALL_SUBDIR <path>]
#       Optional path where runtime libraries were installed. INSTALL_RPATH is only modified if this is provided.
function (nc_setup_runtime_paths TARGET)
    if(NOT UNIX)
        return()
    endif()

    set(options)
    set(oneValueArgs LIBRARY_INSTALL_SUBDIR)
    set(multiValueArgs)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(_DILIGENT_BUILD_RPATH_DIRS_LIST "")
    foreach(DEPENDENCY ${DILIGENT_RUNTIME_LIBRARIES})
        get_target_property(DEPENDENCY_DIR ${DEPENDENCY} BINARY_DIR)
        if(DEPENDENCY_DIR)
            list(APPEND _DILIGENT_BUILD_RPATH_DIRS_LIST "${DEPENDENCY_DIR}")
        endif()
    endforeach()

    list(JOIN _DILIGENT_BUILD_RPATH_DIRS_LIST ";" DILIGENT_BUILD_RPATH_DIRS)

    get_target_property(CURRENT_TARGET_BUILD_RPATH ${TARGET} BUILD_RPATH)

    set_target_properties(${TARGET}
        PROPERTIES
            BUILD_RPATH "${CURRENT_TARGET_BUILD_RPATH};${DILIGENT_BUILD_RPATH_DIRS}"
    )

    if(ARG_LIBRARY_INSTALL_SUBDIR)
        get_target_property(CURRENT_TARGET_INSTALL_RPATH ${TARGET} INSTALL_RPATH)

        set_target_properties(${TARGET}
            PROPERTIES
                INSTALL_RPATH "${CURRENT_TARGET_INSTALL_RPATH};${CMAKE_INSTALL_PREFIX}/${ARG_LIBRARY_INSTALL_SUBDIR}"
        )
    endif()
endfunction()
