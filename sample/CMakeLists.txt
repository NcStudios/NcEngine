# Sample CMake Root
add_executable(${NC_SAMPLE_EXE})

# Compile Options
target_compile_options(${NC_SAMPLE_EXE}
    PRIVATE
        ${NC_COMPILER_FLAGS}
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
if(WIN32)
    # Diligent helper method to get required DLLs and EXEs into the project's executable dir.
    copy_required_dlls(${NC_SAMPLE_EXE})

    install(DIRECTORY      $<TARGET_FILE_DIR:${NC_SAMPLE_EXE}>/
            DESTINATION    sample
            FILES_MATCHING REGEX ".*\.(dll)" # Diligent DLLs and EXEs
)
elseif(UNIX AND NOT APPLE)
    install(TARGETS     Diligent-GraphicsEngineOpenGL-shared Diligent-GraphicsEngineVk-shared
            DESTINATION lib)
    set_target_properties(${NC_SAMPLE_EXE}
        PROPERTIES
            INSTALL_RPATH "$<TARGET_FILE_DIR:${NC_SAMPLE_EXE}>/lib/GraphicsEngineOpenGL;$<TARGET_FILE_DIR:${NC_SAMPLE_EXE}>/lib/GraphicsEngineVulkan"
    )
endif()

install(TARGETS     ${NC_SAMPLE_EXE}
        DESTINATION sample
)
install(DIRECTORY   ${PROJECT_SOURCE_DIR}/sample/assets/nca
        DESTINATION sample
)
install(DIRECTORY      ${PROJECT_SOURCE_DIR}/sample/assets/font
        DESTINATION    sample/nca
        FILES_MATCHING REGEX ".*\.(ttf)"
)
install(DIRECTORY   ${PROJECT_SOURCE_DIR}/resources/nca
        DESTINATION sample
)
install(DIRECTORY      ${PROJECT_SOURCE_DIR}/resources/shaders/compiled/
        DESTINATION    sample/nca/shaders
        FILES_MATCHING REGEX ".*\.(spv)"
)
install(FILES       ${PROJECT_SOURCE_DIR}/sample/config.ini
        DESTINATION sample
)
if(${NC_PROFILING_ENABLED})
    install(FILES       $<TARGET_FILE:OptickCore>
            DESTINATION sample
    )
endif()
