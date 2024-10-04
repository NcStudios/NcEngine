target_sources(nc-convert
    PRIVATE
        ${PROJECT_SOURCE_DIR}/source/ncconvert/converters/AudioConverter.cpp
        ${PROJECT_SOURCE_DIR}/source/ncconvert/converters/GeometryConverter.cpp
        ${PROJECT_SOURCE_DIR}/source/ncconvert/converters/TextureConverter.cpp
)