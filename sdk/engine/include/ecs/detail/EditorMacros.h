#pragma once

#ifdef NC_EDITOR_ENABLED
namespace nc
{
    class UIStream;

    namespace editor
    {
        struct AssetDependencyChecker;
        class Inspector;
    }
}
#define NC_ENABLE_IN_EDITOR(Component)       \
friend ::nc::editor::Inspector;              \
friend ::nc::editor::AssetDependencyChecker; \
friend ::nc::UIStream;
#else
#define NC_ENABLE_IN_EDITOR(Component)
#endif

/** Entity */
#define NC_EDITOR_ENTITY_INFO(...) EntityInfo{__VA_ARGS__}
#define NC_EDITOR_ADD_ENTITY(handle, info) Entity handle = registry->Add<Entity>(info)

/** Audio */
#define NC_EDITOR_REGISTER_AUDIO_LISTENER(handle) engine->Audio()->RegisterAudioListener(handle)

/** Camera */
#define NC_EDITOR_ADD_CAMERA(handle) registry->Add<nc::graphics::Camera>(handle)
#define NC_EDITOR_SET_CAMERA(handle) engine->Graphics()->SetCamera(registry->Get<nc::graphics::Camera>(handle))

/** Collider */
#define NC_EDITOR_BOX_PROPERTIES(...) nc::physics::BoxProperties{__VA_ARGS__}
#define NC_EDITOR_CAPSULE_PROPERTIES(...) nc::physics::CapsuleProperties{__VA_ARGS__}
#define NC_EDITOR_HULL_PROPERTIES(...) nc::physics::HullProperties{__VA_ARGS__}
#define NC_EDITOR_SPHERE_PROPERTIES(...) nc::physics::SphereProperties{__VA_ARGS__}
#define NC_EDITOR_ADD_BOX_COLLIDER(handle, properties, isTrigger) registry->Add<nc::physics::Collider>(handle, properties, isTrigger)
#define NC_EDITOR_ADD_CAPSULE_COLLIDER(handle, properties, isTrigger) registry->Add<nc::physics::Collider>(handle, properties, isTrigger)
#define NC_EDITOR_ADD_HULL_COLLIDER(handle, properties, isTrigger) registry->Add<nc::physics::Collider>(handle, properties, isTrigger)
#define NC_EDITOR_ADD_SPHERE_COLLIDER(handle, properties, isTrigger) registry->Add<nc::physics::Collider>(handle, properties, isTrigger)
#define NC_EDITOR_ADD_CONCAVE_COLLIDER(handle, assetPath) registry->Add<nc::physics::ConcaveCollider>(handle, assetPath)

/** PhysicsBody */
#define NC_EDITOR_PHYSICS_PROPERTIES(...) nc::physics::PhysicsProperties{__VA_ARGS__}
#define NC_EDITOR_ADD_PHYSICS_BODY(handle, properties) registry->Add<nc::physics::PhysicsBody>(handle, properties)

/** PointLight */
#define NC_EDITOR_POINT_LIGHT_INFO(...) nc::graphics::PointLightInfo{__VA_ARGS__}
#define NC_EDITOR_ADD_POINT_LIGHT(handle, info) registry->Add<nc::graphics::PointLight>(handle, info)

/** MeshRenderer */
#define NC_EDITOR_MATERIAL(...) nc::graphics::Material{__VA_ARGS__}
#define NC_EDITOR_ADD_MESH_RENDERER(handle, material, technique)

/** Skybox */
#define NC_EDITOR_ADD_SKYBOX(assetPath) nc::LoadCubeMapAsset(assetPath)
#define NC_EDITOR_SET_SKYBOX(assetPath) engine->Graphics()->SetSkybox(assetPath)
