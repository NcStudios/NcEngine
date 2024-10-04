#pragma once

#include "ncasset/AssetsFwd.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

namespace nc::convert
{
class GeometryConverter
{
    public:
        GeometryConverter();
        ~GeometryConverter() noexcept;

        /** Process an fbx file as geometry for a concave collider. */
        auto ImportConcaveCollider(const std::filesystem::path& path) -> asset::ConcaveCollider;

        /** Process an fbx file as geometry for a hull collider. */
        auto ImportHullCollider(const std::filesystem::path& path) -> asset::HullCollider;

        /** Process an fbx file as geometry for a mesh renderer. Supply a subResourceName of the mesh to extract if there are multiple meshes in the fbx file. */
        auto ImportMesh(const std::filesystem::path& path, const std::optional<std::string>& subResourceName = std::nullopt) -> asset::Mesh;

        /** Process an fbx file into a skeletal animation clip. Supply a subResourceName of the clip to extract if there are multiple clips in the fbx file. */
        auto ImportSkeletalAnimation(const std::filesystem::path& path, const std::optional<std::string>& subResourceName = std::nullopt) -> asset::SkeletalAnimation;

    private:
        class impl;
        std::unique_ptr<impl> m_impl;
};
} // namespace nc::convert
