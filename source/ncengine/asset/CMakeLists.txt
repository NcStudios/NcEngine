target_sources(${NC_ENGINE_LIB}
    PRIVATE
        AssetData.cpp
        Assets.cpp
        NcAssetImpl.cpp
)

add_subdirectory(manager)
