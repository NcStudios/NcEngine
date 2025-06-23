#pragma once

#include "Type.h"

#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/FrameLogic.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/detail/FreeComponentGroup.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/graphics/Light.h"
#include "ncengine/graphics/Mesh.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/physics/CollisionListener.h"
#include "ncengine/physics/RigidBody.h"

namespace nc::type
{
REGISTER_TYPE(Vector2, PROPERTY(Vector2, x), PROPERTY(Vector2, y));
REGISTER_TYPE(Vector3, PROPERTY(Vector3, x), PROPERTY(Vector3, y), PROPERTY(Vector3, z));
REGISTER_TYPE(Vector4, PROPERTY(Vector4, x), PROPERTY(Vector4, y), PROPERTY(Vector4, z), PROPERTY(Vector4, w));
REGISTER_TYPE(Quaternion, PROPERTY(Quaternion, x), PROPERTY(Quaternion, y), PROPERTY(Quaternion, z), PROPERTY(Quaternion, w));

/** @todo Only registering names of these: */
REGISTER_TYPE(Entity);
REGISTER_TYPE(FrameLogic);
REGISTER_TYPE(Tag);
REGISTER_TYPE(Transform);
REGISTER_TYPE(AudioSource);
REGISTER_TYPE(ecs::detail::FreeComponentGroup);
REGISTER_TYPE(Camera);
REGISTER_TYPE(DirectionalLight);
REGISTER_TYPE(ParticleEmitter);
REGISTER_TYPE(PointLight);
REGISTER_TYPE(SpotLight);
REGISTER_TYPE(StaticMesh);
REGISTER_TYPE(SkinnedMesh);
REGISTER_TYPE(CollisionListener);
REGISTER_TYPE(Constraint);
REGISTER_TYPE(RigidBody);
REGISTER_TYPE(Shape);
} // namespace nc::type
