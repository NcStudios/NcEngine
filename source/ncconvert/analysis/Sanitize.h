#pragma once

#include "ncasset/Assets.h"

#include <span>

namespace nc::convert
{
auto Sanitize(float* in) -> size_t;
auto Sanitize(Vector2* in) -> size_t;
auto Sanitize(Vector3* in) -> size_t;
auto Sanitize(asset::MeshVertex* in) -> size_t;
auto Sanitize(std::span<Vector2> in) -> size_t;
auto Sanitize(std::span<Vector3> in) -> size_t;
auto Sanitize(std::span<asset::MeshVertex> in) -> size_t;
auto Sanitize(std::span<Triangle> in) -> size_t;
} // namespace nc::convert
