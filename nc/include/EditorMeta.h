#pragma once

#ifdef NC_EDITOR_ENABLED
namespace nc::editor { class Inspector; }
#define NC_ENABLE_IN_EDITOR(Component) friend ::nc::editor::Inspector;
#else
#define NC_ENABLE_IN_EDITOR(Component)
#endif

#define NC_SCENE_ACTION_ADD_ENTITY(handle, pos, rot, scl, parent, tag, layer, flags) \
Entity handle = registry->Add<Entity>(EntityInfo{.position = pos, .rotation = rot, .scale = scl, .parent = parent, .tag = tag, .layer = layer, .flags = flags});

#define NC_SCENE_ACTION_ADD_BOX_COLLIDER(handle, center, extents, isTrigger) \
registry->Add<Collider>(handle, BoxProperties{.center = center, .extents = extents}, isTrigger);

#define NC_SCENE_ACTION_ADD_CAPSULE_COLLIDER(handle, center, height, radius, isTrigger) \
registry->Add<Collider>(handle, CapsuleProperties{.center = center, .height = height, .radius = radius}, isTrigger);

#define NC_SCENE_ACTION_ADD_HULL_COLLIDER(handle, assetPath, isTrigger) \
registry->Add<Collider>(handle, HullProperties{.assetPath = assetPath}, isTrigger);

#define NC_SCENE_ACTION_ADD_SPHERE_COLLIDER(handle, center, radius, isTrigger) \
registry->Add<Collider>(handle, SpherePropertiess{.center = center, .radius = radius}, isTrigger);

#define NC_SCENE_ACTION_ADD_PHYSICS_BODY(handle, mass, drag, angularDrag, restitution, friction, useGravity, isKinematic) \
registry->Add<PhysicsBody>(handle, PhysicsProperties{.mass = mass, .drag = drag, .angularDrag = angularDrag, .restitution = restitution, .friction = friction, .useGravity = useGravity, .isKinematic = isKinematic});

#define NC_SCENE_ACTION_ADD_POINT_LIGHT(handle, pos, ambient, diffuseColor, diffuseIntensity, attConst, attLin, attQuad) \
registry->Add<PointLight>(handle, PointLightInfo{.pos = pos, .ambient = ambient, .diffuseColor = diffuseColor, .diffuseIntensity = diffuseIntensity, .attConst = attConst, .attLin = attLin, .attQuad = attQuad});

#define NC_SCENE_ACTION_ADD_MESH_RENDERER(handle, meshPath, baseColor, normal, roughness, technique) \
registry->Add<MeshRenderer>(handle, meshPath, graphics::Material{.basColor = baseColor, .normal = normal, .roughness = roughness}, technique);
