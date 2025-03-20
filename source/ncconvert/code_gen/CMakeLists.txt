target_sources(nc-convert
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ncconvert/code_gen/PrefabGenerator.cpp



        # TODO: don't want to do this
        ${PROJECT_SOURCE_DIR}/source/ncengine/asset/DefaultAssets.cpp
)
