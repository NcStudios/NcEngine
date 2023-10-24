/**
 * @file NcAsset.h
 * @copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

namespace nc::asset
{
constexpr auto PlaneMesh = "plane.nca";
constexpr auto CubeMesh = "cube.nca";
constexpr auto SphereMesh = "sphere.nca";
constexpr auto CapsuleMesh = "capsule.nca";
constexpr auto SkyboxMesh = "skybox.nca";

constexpr auto DefaultBaseColor = "DefaultBaseColor.nca";
constexpr auto DefaultNormal = "DefaultNormal.nca";
constexpr auto DefaultParticle = "DefaultParticle.nca";
constexpr auto DefaultCubeMap = "DefaultSkybox.nca";
constexpr auto DefaultAudioClip = "empty.nca";
constexpr auto DefaultConcaveCollider = "plane.nca";
constexpr auto DefaultHullCollider = "cube.nca";
constexpr auto DefaultMesh = CubeMesh;
} // namespace nc::aset
