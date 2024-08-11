
#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

struct ObjectData
{
    mat4 model;

    // Textures
    uint baseColorIndex;
    uint normalIndex;
    uint roughnessIndex;
    uint metallicIndex;

    uint skeletalAnimationIndex;
};

struct SkeletalAnimationData
{
    mat4 finalTransform;
};

layout(std140, set=0, binding=0) readonly buffer ObjectBuffer
{
    ObjectData objects[];
} objectBuffer;

layout (std140, set=0, binding=6) readonly buffer AnimatedBoneTransforms
{
    SkeletalAnimationData animatedBones[];
} skeletalAnimationBuffer;

layout (location = 0) out vec2 outUV;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
	outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(outUV.xy * 2.0f - 1.0f, 0.0f, 1.0f);
}