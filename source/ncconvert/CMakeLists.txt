add_executable(nc-convert)

target_compile_options(nc-convert
    PRIVATE
        ${NC_COMPILER_FLAGS}
)

target_compile_definitions(nc-convert
    PRIVATE
        ${NC_COMPILE_DEFINITIONS}
)

target_include_directories(nc-convert
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ncconvert
)

target_include_directories(nc-convert
    SYSTEM PRIVATE
        ${PROJECT_SOURCE_DIR}/source/external
)

target_sources(nc-convert
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ncconvert/Main.cpp
)

target_link_libraries(nc-convert
    PRIVATE
        NcAsset
        NcMath
        NcUtility
        assimp::assimp
)

install(
    TARGETS
        nc-convert
    DESTINATION
        bin
)

add_subdirectory(analysis)
add_subdirectory(builder)
add_subdirectory(converters)
add_subdirectory(utility)
