/**
 * @file Geometry.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "Vector.h"

#include <span>

namespace nc
{
struct Sphere
{
    Vector3 center;
    float radius;
};

struct Box
{
    Vector3 center;
    Vector3 extents;
    float maxExtent;
};

struct Capsule
{
    Vector3 pointA, pointB;
    float radius;
    float maxExtent;
};

struct ConvexHull
{
    std::span<const Vector3> vertices;
    Vector3 extents;
    float maxExtent;
};

struct Triangle
{
    Vector3 a, b, c;
};

struct Plane
{
    Vector3 normal;
    float d;
};

struct Frustum
{
    Plane left;
    Plane right;
    Plane bottom;
    Plane top;
    Plane front;
    Plane back;
};
} // namespace nc
