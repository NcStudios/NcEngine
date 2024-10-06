target_sources(${NC_ENGINE_LIB}
    PRIVATE
        Device.cpp
        GpuOptions.cpp
        Instance.cpp
)

add_subdirectory(detail)
