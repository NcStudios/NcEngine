target_sources(${EDITOR_EXE}
    PRIVATE
        ${ROOT_DIR}/source/assets/Asset.cpp
        ${ROOT_DIR}/source/assets/AssetCollection.cpp
        ${ROOT_DIR}/source/assets/AssetManifest.cpp
        ${ROOT_DIR}/source/assets/AssetDependencyChecker.cpp
        ${ROOT_DIR}/source/assets/ManifestSerializer.cpp
)