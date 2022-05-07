# NcEditor CMake Root
if(NC_PROD_BUILD)
    message(FATAL_ERROR "Error in CMake - Attempting to build NcEditor with NC_PROD_BUILD=ON")
endif()

add_executable(${NC_EDITOR_EXE})

# Compile Options
target_compile_options(${NC_EDITOR_EXE}
    PUBLIC
        ${WARNING_FLAGS}
)

target_compile_definitions(${NC_EDITOR_EXE}
    PUBLIC
        ${NC_COMPILE_DEFINITIONS}
)

# Include Dirs
target_include_directories(${NC_EDITOR_EXE}
    PRIVATE
        source
        ${NC_INCLUDE_DIR}
        ${NC_EXTERNAL_DIR}
)

# Link Options
target_link_libraries(${NC_EDITOR_EXE}
    PRIVATE
        ${NC_ENGINE_LIB}
)

add_subdirectory(source)