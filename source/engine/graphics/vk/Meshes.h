#include "graphics/Base.h"
#include "graphics/GpuAllocator.h"
#include "graphics/resources/ImmutableBuffer.h"

#include <span>

namespace nc::graphics
{
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
} // namespace nc::graphics