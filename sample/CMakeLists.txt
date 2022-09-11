# Sample CMake Root
add_executable(${NC_SAMPLE_EXE})

# Compile Options
target_compile_options(${NC_SAMPLE_EXE}
    PRIVATE
        ${WARNING_FLAGS}
)

target_compile_definitions(${NC_SAMPLE_EXE}
    PRIVATE
        ${NC_COMPILE_DEFINITIONS}
)

# Include Dirs
target_include_directories(${NC_SAMPLE_EXE}
    PRIVATE
        source
        ${NC_INCLUDE_DIR}
        ${NC_EXTERNAL_DIR}
)

# Link Options
target_link_libraries(${NC_SAMPLE_EXE}
    PRIVATE
        ${NC_ENGINE_LIB}
)

add_subdirectory(source)

# Install
install(TARGETS     ${NC_SAMPLE_EXE}
        DESTINATION sample
)
install(DIRECTORY   ${PROJECT_SOURCE_DIR}/sample/assets
        DESTINATION sample
)
install(FILES       ${PROJECT_SOURCE_DIR}/sample/config.ini
        DESTINATION sample
)
if(${NC_PROFILING_ENABLED})
    install(FILES       ${PROJECT_SOURCE_DIR}/source/external/lib/OptickCore.dll
            DESTINATION sample
    )
endif()