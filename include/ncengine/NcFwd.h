/**
 * @file NcFwd.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

namespace nc
{
class NcEngine;
class Registry;
class ModuleRegistry;
class Entity;
class Module;
class AnyComponent;
class ComponentBase;
class FreeComponent;
class FrameLogic;
struct Tag;
class Transform;
struct Quaternion;
struct Vector2;
struct Vector3;
struct Vector4;

// Physics Types
struct NcPhysics;
struct CollisionListener;
class Constraint;
class RigidBody;
struct Shape;

namespace audio
{
struct NcAudio;
class AudioSource;
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
struct NcGraphics;
class Camera;
class MeshRenderer;
class ParticleEmitter;
struct PointLight;
class SceneNavigationCamera;
class SkeletalAnimator;
struct SpotLight;
class ToonRenderer;
} // namespace graphics

namespace net
{
class NetworkDispatcher;
} // namespace net
} // namespac nc
