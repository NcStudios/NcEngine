target_sources(${NC_ENGINE_LIB}
    PRIVATE
        NcGraphicsImpl2.cpp
)

if (WIN32)
    target_sources(${NC_ENGINE_LIB}
        PRIVATE
            NcGraphicsImpl2Win32.cpp
    )
endif()
if (UNIX AND NOT APPLE)
    target_sources(${NC_ENGINE_LIB}
        PRIVATE
            NcGraphicsImpl2Linux.cpp
    )
endif()

add_subdirectory(diligent)
