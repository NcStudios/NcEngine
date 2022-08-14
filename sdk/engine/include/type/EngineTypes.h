#pragma once

#include "Type.h"
#include "ecs/All.h"

namespace nc::type
{
REGISTER_TYPE(Vector2, PROPERTY(Vector2, x), PROPERTY(Vector2, y));
REGISTER_TYPE(Vector3, PROPERTY(Vector3, x), PROPERTY(Vector3, y), PROPERTY(Vector3, z));
REGISTER_TYPE(Vector4, PROPERTY(Vector4, x), PROPERTY(Vector4, y), PROPERTY(Vector4, z), PROPERTY(Vector4, w));
REGISTER_TYPE(Quaternion, PROPERTY(Quaternion, x), PROPERTY(Quaternion, y), PROPERTY(Quaternion, z), PROPERTY(Quaternion, w));
REGISTER_TYPE(graphics::PointLightInfo, PROPERTY(graphics::PointLightInfo, ambient), PROPERTY(graphics::PointLightInfo, diffuseColor), PROPERTY(graphics::PointLightInfo, diffuseIntensity));

/** @todo Only registering names of these: */
REGISTER_TYPE(CollisionLogic);
REGISTER_TYPE(Entity);
REGISTER_TYPE(FixedLogic);
REGISTER_TYPE(FrameLogic);
REGISTER_TYPE(Tag);
REGISTER_TYPE(Transform);
REGISTER_TYPE(audio::AudioSource);
REGISTER_TYPE(ecs::detail::FreeComponentGroup);
REGISTER_TYPE(graphics::Camera);
REGISTER_TYPE(graphics::MeshRenderer);
REGISTER_TYPE(graphics::ParticleEmitter);
REGISTER_TYPE(graphics::PointLight);
REGISTER_TYPE(net::NetworkDispatcher);
REGISTER_TYPE(physics::ConcaveCollider);
REGISTER_TYPE(physics::Collider);
REGISTER_TYPE(physics::PhysicsBody);
} // namespace nc::type
