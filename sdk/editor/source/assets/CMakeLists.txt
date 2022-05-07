target_sources(${EDITOR_EXE}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/assets/Asset.cpp
        ${PROJECT_SOURCE_DIR}/source/assets/AssetCollection.cpp
        ${PROJECT_SOURCE_DIR}/source/assets/AssetManifest.cpp
        ${PROJECT_SOURCE_DIR}/source/assets/AssetDependencyChecker.cpp
        ${PROJECT_SOURCE_DIR}/source/assets/ManifestSerializer.cpp
)