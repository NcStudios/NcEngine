/**
 * @file AssetLoader.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

namespace nc::asset
{
struct ConvexHull;
struct NamedMesh;

/** @brief Interface for loading and building physics assets. */
class PhysicsAssetLoader
{
    public:
        virtual ~PhysicsAssetLoader() = default;

        /** @brief Convert a mesh to a ConvexHull on demand. */
        virtual auto ConvertToConvexHull(const asset::NamedMesh& mesh) const -> asset::ConvexHull = 0;
};
} // namespace nc::asset
