bool IsValidBoneIndex(uint boneIndex)
{
    return boneIndex != 4294967295;
}

bool IsValidAnimationTransform(float4x4 mat)
{
    // Check for zero matrix, ignoring homogenous coord
    return
        any(mat[0]) ||
        any(mat[1]) ||
        any(mat[2]) ||
        any(mat[3].xyz);
}

float4x4 CombineBoneMatrices(uint base, uint4 boneOffsets, float4 boneWeights)
{
    float4x4 boneTransform = float4x4(0.0, 0.0, 0.0, 0.0,
                                      0.0, 0.0, 0.0, 0.0,
                                      0.0, 0.0, 0.0, 0.0,
                                      0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < 4; i++)
    {
        if (boneWeights[i] > 0.0f)
        {
            boneTransform += Bones[base + boneOffsets[i]].animatedBoneMatrix * boneWeights[i];
        }
    }

    return boneTransform;
}
