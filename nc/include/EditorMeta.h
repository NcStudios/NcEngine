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
#define NC_EDITOR_REGISTER_AUDIO_LISTENER(handle) engine->AudioSystem()->RegisterAudioListener(handle)

/** Camera */
#define NC_EDITOR_ADD_CAMERA(handle) registry->Add<Camera>(handle)
#define NC_EDITOR_SET_CAMERA(handle) engine->MainCamera()->Set(registry->Get<Camera>(handle));

/** Collider */
#define NC_EDITOR_BOX_PROPERTIES(...) BoxProperties{__VA_ARGS__}
#define NC_EDITOR_CAPSULE_PROPERTIES(...) CapsuleProperties{__VA_ARGS__}
#define NC_EDITOR_HULL_PROPERTIES(...) HullProperties{__VA_ARGS__}
#define NC_EDITOR_SPHERE_PROPERTIES(...) SphereProperties{__VA_ARGS__}
#define NC_EDITOR_ADD_BOX_COLLIDER(handle, properties, isTrigger) registry->Add<Collider>(handle, properties, isTrigger)
#define NC_EDITOR_ADD_CAPSULE_COLLIDER(handle, properties, isTrigger) registry->Add<Collider>(handle, properties, isTrigger)
#define NC_EDITOR_ADD_HULL_COLLIDER(handle, properties, isTrigger) registry->Add<Collider>(handle, properties, isTrigger)
#define NC_EDITOR_ADD_SPHERE_COLLIDER(handle, properties, isTrigger) registry->Add<Collider>(handle, properties, isTrigger)
#define NC_EDITOR_ADD_CONCAVE_COLLIDER(handle, assetPath) registry->Add<ConcaveCollider>(handle, assetPath);

/** PhysicsBody */
#define NC_EDITOR_PHYSICS_PROPERTIES(...) PhysicsProperties{__VA_ARGS__}
#define NC_EDITOR_ADD_PHYSICS_BODY(handle, properties) registry->Add<PhysicsBody>(handle, properties)

/** PointLight */
#define NC_EDITOR_POINT_LIGHT_INFO(...) PointLightInfo{__VA_ARGS__}
#define NC_EDITOR_ADD_POINT_LIGHT(handle, info) registry->Add<PointLight>(handle, info)

/** MeshRenderer */
#define NC_EDITOR_MATERIAL(...) Material{__VA_ARGS__}
#define NC_EDITOR_ADD_MESH_RENDERER(handle, material, technique)