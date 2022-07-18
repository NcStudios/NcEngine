#pragma once

#include "resources/ImmutableBuffer.h"
#include "resources/ImmutableImage.h"

namespace nc
{
namespace graphics
{

/** Meshes **/
struct VertexBuffer
{
    VertexBuffer(Base* base, GpuAllocator* allocator);
    std::span<const nc::Vertex> vertices;
    ImmutableBuffer buffer;
};

struct IndexBuffer
{
    IndexBuffer(Base* base, GpuAllocator* allocator);
    std::span<const uint32_t> indices;
    ImmutableBuffer buffer;
};

/** Textures **/
struct TextureBuffer
{
    ImmutableImage image;
    std::string uid;
};

struct TextureImageInfo
{
    vk::DescriptorImageInfo imageInfo;
};

}
}