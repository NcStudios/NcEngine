/**
 * @file DefaultAssets.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 * 
 * @note Assets in this file are guaranteed to be loaded before each call to
 *       `Scene::Load()`.
 */
#pragma once

#include "ncasset/AssetType.h"

#include <span>
#include <string_view>

namespace nc::asset
{
/** @brief Name of the default plane mesh asset. */
constexpr auto PlaneMesh = "default/plane.nca";

/** @brief Name of the default cube mesh asset. */
constexpr auto CubeMesh = "default/cube.nca";

/** @brief Name of the default sphere mesh asset. */
constexpr auto SphereMesh = "default/sphere.nca";

/** @brief Name of the default capsule mesh asset. */
constexpr auto CapsuleMesh = "default/capsule.nca";

/** @brief Name of the default skybox mesh asset. */
constexpr auto SkyboxMesh = "default/skybox.nca";

/** @brief Name of the default wheel mesh asset. */
constexpr auto WheelMesh = "default/wheel.nca";

/** @brief Name of the default base color texture asset. */
constexpr auto DefaultBaseColor = "default/color.nca";

/** @brief Name of the default normal texture asset. */
constexpr auto DefaultNormal = "default/normal.nca";

/** @brief Name of the default particle texture asset. */
constexpr auto DefaultParticle = "default/particle.nca";

/** @brief Name of the default shape key animation asset; a blowing flag. */
constexpr auto DefaultShapeKeyAnimation = "default/shapekey.nca";

/** @brief Name of the default skeletal animation asset; a spinning cube. */
constexpr auto DefaultSkeletalAnimation = "default/cube.nca";

/** @brief Name of the default skybox cubemap asset. */
constexpr auto DefaultSkyboxCubeMap = "default/skybox.nca";

/** @brief Name of the default empty audio clip asset. */
constexpr auto DefaultAudioClip = "default/silence.nca";

/** @brief Name of the default plane-shaped mesh collider asset. */
constexpr auto DefaultMeshCollider = "default/plane.nca";

/** @brief Name of the default cube-shaped convex hull asset. */
constexpr auto DefaultConvexHull = "default/cube.nca";

/** @name Get names of all default assets of a particular type */
auto GetDefaultAudioClipPaths() -> std::span<const std::string_view>;
auto GetDefaultConvexHullPaths() -> std::span<const std::string_view>;
auto GetDefaultCubeMapPaths() -> std::span<const std::string_view>;
auto GetDefaultMeshPaths() -> std::span<const std::string_view>;
auto GetDefaultMeshColliderPaths() -> std::span<const std::string_view>;
auto GetDefaultShapeKeyAnimationPaths() -> std::span<const std::string_view>;
auto GetDefaultSkeletalAnimationPaths() -> std::span<const std::string_view>;
auto GetDefaultTexturePaths() -> std::span<const std::string_view>;
} // namespace nc::asset
