struct ShapeKeyMetadata
{
    int shapeKeyAnimationIndex;
    float durationInSeconds;
    uint numShapeKeys;
    uint padding;
};

bool IsValidShapeKeyIndex(uint shapeKeyIndex)
{
    return shapeKeyIndex != 4294967295;
}

float3 ApplyShapeKeyAnimation(uint shapeKeyMetadataIndex, ShapeKeyMetadata metadata, Texture2D<float> shapeKeyAnimationData, float time, int vertexID)
{
    float wrappedTime = fmod(time, metadata.durationInSeconds);
    float normalizedT = wrappedTime / metadata.durationInSeconds;
    float frameF = normalizedT * (metadata.numShapeKeys - 1); 
    uint shapeKeyIndexLow = (uint)floor(frameF);
    uint shapeKeyIndexHigh = min(shapeKeyIndexLow + 1, metadata.numShapeKeys - 1);
    float shapeKeyLerpFactor = frac(frameF);

    int3 texelCoords = int3(vertexID * 3 + 0, shapeKeyIndexLow, 0); // X
    float positionOffsetLowX = shapeKeyAnimationData.Load(texelCoords); // X, Y, Z, X, Y, Z, X, Y, Z
    texelCoords.x += 1; // Y
    float positionOffsetLowY = shapeKeyAnimationData.Load(texelCoords);
    texelCoords.x += 1; // Z
    float positionOffsetLowZ = shapeKeyAnimationData.Load(texelCoords);

    texelCoords = int3(vertexID * 3 + 0, shapeKeyIndexHigh, 0); // X
    float positionOffsetHighX = shapeKeyAnimationData.Load(texelCoords); // X, Y, Z, X, Y, Z, X, Y, Z
    texelCoords.x += 1; // Y
    float positionOffsetHighY = shapeKeyAnimationData.Load(texelCoords);
    texelCoords.x += 1; // Z
    float positionOffsetHighZ = shapeKeyAnimationData.Load(texelCoords);

    return float3(lerp(positionOffsetLowX, positionOffsetHighX, shapeKeyLerpFactor), lerp(positionOffsetLowY, positionOffsetHighY, shapeKeyLerpFactor), lerp(positionOffsetLowZ, positionOffsetHighZ, shapeKeyLerpFactor));;
}
