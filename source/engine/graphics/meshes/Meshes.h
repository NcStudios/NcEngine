#include "graphics/resources/ImmutableBuffer.h"

#include <span>

namespace nc::graphics
{
class GpuAllocator; 

struct VertexBuffer
{
    VertexBuffer(GpuAllocator* allocator);
    std::span<const nc::Vertex> vertices;
    ImmutableBuffer buffer;
};

struct IndexBuffer
{
    IndexBuffer(GpuAllocator* allocator);
    std::span<const uint32_t> indices;
    ImmutableBuffer buffer;
};
} // namespace nc::graphics