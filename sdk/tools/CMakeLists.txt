# Fetch Assimp
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(assimp
                     GIT_REPOSITORY https://github.com/NcStudios/assimp.git
                     GIT_TAG        origin/master
)

FetchContent_MakeAvailable(assimp)

add_executable(nc-tools)

target_compile_options(nc-tools
    PRIVATE
        ${WARNING_FLAGS}
)

target_link_libraries(nc-tools
    PRIVATE
        assimp::assimp
)

target_sources(nc-tools
    PRIVATE
        NcTools.cpp
)

install(TARGETS     nc-tools
        DESTINATION bin
)

install(FILES       shaders.ps1
        DESTINATION scripts
)