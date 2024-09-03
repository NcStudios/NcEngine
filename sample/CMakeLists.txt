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

# Define the directory where the DLLs are located.
set(STRIP_DOUBLE_D_DIR "${CMAKE_BINARY_DIR}/sample/${CMAKE_BUILD_TYPE}/")

# Find all DLL files in the specified directory.
file(GLOB DLL_FILES "${STRIP_DOUBLE_D_DIR}/*.dll")

# Loop over each DLL file found.
foreach(DLL_FILE ${DLL_FILES})
    # Get the filename without the directory path.
    get_filename_component(DLL_FILENAME ${DLL_FILE} NAME)
    
    # Check if the filename ends with "d.dll".
    string(REGEX REPLACE "(.*)d\\.dll$" "\\1.dll" NEW_FILENAME ${DLL_FILENAME})

    # Rename the file only if the last character before ".dll" is "d".
    if(NOT "${DLL_FILENAME}" STREQUAL "${NEW_FILENAME}")
        # Construct the full paths for the old and new filenames.
        set(OLD_PATH "${STRIP_DOUBLE_D_DIR}/${DLL_FILENAME}")
        set(NEW_PATH "${STRIP_DOUBLE_D_DIR}/${NEW_FILENAME}")
        
        # Rename the file.
        file(RENAME "${OLD_PATH}" "${NEW_PATH}")
        message(STATUS "Renamed: ${OLD_PATH} -> ${NEW_PATH}")
    endif()
endforeach()

# Install
install(DIRECTORY           ${CMAKE_BINARY_DIR}/sample/${CMAKE_BUILD_TYPE}/
        DESTINATION         sample
        FILES_MATCHING REGEX ".*\.(dll)"
)

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
install(DIRECTORY      ${PROJECT_SOURCE_DIR}/resources/shaders/
        DESTINATION    sample/nca/shaders
)
install(FILES       ${PROJECT_SOURCE_DIR}/sample/config.ini
        DESTINATION sample
)
if(${NC_PROFILING_ENABLED})
    install(FILES       $<TARGET_FILE:OptickCore>
            DESTINATION sample
    )
endif()
