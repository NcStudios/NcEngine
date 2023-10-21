/**
 * @file NcFwd.h
 * @copyright Jaremie and McCallister Romer 2023
 */
#pragma once

namespace nc
{
// General
class NcEngine;
class Registry;
class Entity;
struct EntityInfo;
struct Module;
struct Vector2;
struct Vector3;
struct Vector4;
struct Quaternion;
// Components
class AnyComponent;
class ComponentBase;
class FreeComponent;
class CollisionLogic;
class FixedLogic;
class FrameLogic;
class Tag;
class Transform;
// Asset Views
struct AudioClipView;
struct ConcaveColliderView;
struct ConvexHullView;
struct MeshView;
struct TextureView;
struct CubeMapView;
struct ShaderView;

namespace audio
{
// Module
struct NcAudio;
// Components
class AudioSource;
// Init Params
struct AudioSourceProperties;
} // namespace audio

namespace config
{
struct Config;
struct AssetSettings;
struct AudioSettings;
struct MemorySettings;
struct GraphicsSettings;
struct PhysicsSettings;
struct ProjectSettings;
} // namespace config

namespace graphics
{
// Module
struct NcGraphics;
// Components
class Camera;
class MeshRenderer;
class ParticleEmitter;
class PointLight;
class SceneNavigationCamera;
class ToonRenderer;
// Init Params
struct CameraProperties;
struct ParticleInfo;
struct PbrMaterial;
struct PbrMaterialView;
struct SceneCameraConfig;
struct ToonMaterial;
struct ToonMaterialView;
} // namespace graphics

namespace net
{
// Components
class NetworkDispatcher;
} // namespace net

namespace physics
{
// Module
struct NcPhysics;
// Components
class Collider;
class ConcaveCollider;
class PhysicsBody;
// Init Params
struct BoxProperties;
struct CapsuleProperties;
struct HullProperties;
struct SphereProperties;
struct VolumeInfo;
struct PhysicsProperties;
} // namespace physics
} // namespac nc
