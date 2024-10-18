add_library(NcUtility STATIC)

target_sources(NcUtility
    PRIVATE
        Compression.cpp
        $<TARGET_OBJECTS:lz4>
)

target_compile_definitions(NcUtility
    PRIVATE
        ${NC_COMPILE_DEFINITIONS}
)

target_compile_options(NcUtility
    PRIVATE
        ${NC_COMPILER_FLAGS}
)

target_link_libraries(NcUtility
    PUBLIC
        fmt::fmt
)

target_include_directories(NcUtility
    PUBLIC
        $<INSTALL_INTERFACE:include/>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/external>
)
