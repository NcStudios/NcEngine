/**
 * @file NcAsset.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 * 
 * @note Assets in this file are guaranteed to be loaded before each call to
 *       `Scene::Load()`.
 */
#pragma once

namespace nc::asset
{
/** @brief Name of the default plane mesh asset. */
constexpr auto PlaneMesh = "plane.nca";

/** @brief Name of the default cube mesh asset. */
constexpr auto CubeMesh = "cube.nca";

/** @brief Name of the default sphere mesh asset. */
constexpr auto SphereMesh = "sphere.nca";

/** @brief Name of the default capsule mesh asset. */
constexpr auto CapsuleMesh = "capsule.nca";

/** @brief Name of the default skybox mesh asset. */
constexpr auto SkyboxMesh = "skybox.nca";

/** @brief Name of the default base color texture asset. */
constexpr auto DefaultBaseColor = "DefaultBaseColor.nca";

/** @brief Name of the default normal texture asset. */
constexpr auto DefaultNormal = "DefaultNormal.nca";

/** @brief Name of the default particle texture asset. */
constexpr auto DefaultParticle = "DefaultParticle.nca";

/** @brief Name of the default cubemap asset. */
constexpr auto DefaultCubeMap = "DefaultSkybox.nca";

/** @brief Name of the default audio clip asset. */
constexpr auto DefaultAudioClip = "empty.nca";

/** @brief Name of the default concave collider asset. */
constexpr auto DefaultConcaveCollider = "plane.nca";

/** @brief Name of the default hull collider asset. */
constexpr auto DefaultHullCollider = "cube.nca";
} // namespace nc::asset
