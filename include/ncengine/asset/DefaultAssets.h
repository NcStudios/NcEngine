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
constexpr auto PlaneMesh = "DefaultPlane.nca";

/** @brief Name of the default cube mesh asset. */
constexpr auto CubeMesh = "DefaultCube.nca";

/** @brief Name of the default sphere mesh asset. */
constexpr auto SphereMesh = "DefaultSphere.nca";

/** @brief Name of the default capsule mesh asset. */
constexpr auto CapsuleMesh = "DefaultCapsule.nca";

/** @brief Name of the default skybox mesh asset. */
constexpr auto SkyboxMesh = CubeMesh;

/** @brief Name of the default base color texture asset. */
constexpr auto DefaultBaseColor = "DefaultBaseColor.nca";

/** @brief Name of the default normal texture asset. */
constexpr auto DefaultNormal = "DefaultNormal.nca";

/** @brief Name of the default particle texture asset. */
constexpr auto DefaultParticle = "DefaultParticle.nca";

/** @brief Name of the default cube map asset. */
constexpr auto DefaultCubeMap = "DefaultSkybox.nca";

/** @brief Name of the default empty audio clip asset. */
constexpr auto DefaultAudioClip = "DefaultAudioClip.nca";

/** @brief Name of the default plane-shaped concave collider asset. */
constexpr auto DefaultConcaveCollider = "DefaultConcaveCollider.nca";

/** @brief Name of the default cube-shaped hull collider asset. */
constexpr auto DefaultHullCollider = "DefaultHullCollider.nca";
} // namespace nc::asset
