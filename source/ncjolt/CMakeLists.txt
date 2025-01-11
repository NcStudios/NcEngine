add_library(NcJolt STATIC
    JoltApi.cpp
)

target_compile_options(NcJolt
    PRIVATE
        ${NC_COMPILER_FLAGS}
)

target_compile_definitions(NcJolt
    PRIVATE
        ${NC_COMPILE_DEFINITIONS}
)

target_include_directories(NcJolt
    PUBLIC
        ${PROJECT_SOURCE_DIR}/source/ncjolt
)

target_link_libraries(NcJolt
    PUBLIC
        NcMath
        NcUtility
        Jolt
)
