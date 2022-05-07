# Sample CMake Root
add_executable(${NC_SAMPLE_EXE})

# Compile Options
target_compile_options(${NC_SAMPLE_EXE}
    PRIVATE
        ${WARNING_FLAGS}
)

if(NOT NC_PROD_BUILD)
    target_compile_definitions(${NC_SAMPLE_EXE}
        PRIVATE
            ${NC_COMPILE_DEFINITIONS}
    )
endif()

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

target_sources(${NC_SAMPLE_EXE}
    PRIVATE
        SampleMain.cpp
)

add_subdirectory(source/scenes)
add_subdirectory(source/shared)