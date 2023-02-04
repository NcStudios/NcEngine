target_sources(${NC_ENGINE_LIB}
    PRIVATE
        AssetData.cpp
        AssetManagers.cpp
        Assets.cpp
        AssetUtilities.cpp
)

add_subdirectory(manager)
