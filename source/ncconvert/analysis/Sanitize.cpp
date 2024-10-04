#include "Sanitize.h"

#include <cmath>

namespace nc::convert
{
auto Sanitize(float* in) -> size_t
{
    if (std::isnan(*in))
    {
        *in = 0.0f;
        return 1;
    }

    return 0;
}

auto Sanitize(Vector2* in) -> size_t
{
    return Sanitize(&in->x) + Sanitize(&in->y);
}

auto Sanitize(Vector3* in) -> size_t
{
    return Sanitize(&in->x) + Sanitize(&in->y) + Sanitize(&in->z);
}

auto Sanitize(asset::MeshVertex* in) -> size_t
{
    return Sanitize(&in->position) + Sanitize(&in->normal) + Sanitize(&in->uv)
         + Sanitize(&in->tangent) + Sanitize(&in->bitangent);
}

auto Sanitize(std::span<Vector2> in) -> size_t
{
    auto sanitizedCount = size_t{};
    for(auto& vec : in)
    {
        sanitizedCount += Sanitize(&vec);
    }

    return sanitizedCount;
}

auto Sanitize(std::span<Vector3> in) -> size_t
{
    auto sanitizedCount = size_t{};
    for(auto& vec : in)
    {
        sanitizedCount += Sanitize(&vec);
    }

    return sanitizedCount;
}

auto Sanitize(std::span<asset::MeshVertex> in) -> size_t
{
    auto sanitizedCount = size_t{};
    for(auto& ver : in)
    {
        sanitizedCount += Sanitize(&ver);
    }

    return sanitizedCount;
}

auto Sanitize(std::span<Triangle> in) -> size_t
{
    auto sanitizedCount = size_t{};
    for(auto& tri : in)
    {
        sanitizedCount += Sanitize(&tri.a) +  Sanitize(&tri.b) +  Sanitize(&tri.c);
    }

    return sanitizedCount;
}
} // namespace nc::convert
