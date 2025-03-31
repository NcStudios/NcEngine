/**
 * @file AssetsFwd.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include <cstdint>

namespace nc::asset
{
struct AudioClip;
struct BonesData;
struct ConvexHull;
struct CubeMap;
struct Mesh;
struct MeshCollider;
struct MeshVertex;
struct SkeletalAnimation;
struct Texture;
struct TextureSubResource;

enum class AssetType : int;
enum class AssetSubtype : uint8_t;
enum class TextureFormat : uint8_t;
} // namespace nc::asset
