#pragma once

#include "Type.h"

#include "ncengine/audio/AudioSource.h"
#include "ncengine/ecs/Logic.h"
#include "ncengine/ecs/Tag.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/ecs/detail/FreeComponentGroup.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/graphics/MeshRenderer.h"
#include "ncengine/graphics/ParticleEmitter.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/SkeletalAnimator.h"
#include "ncengine/graphics/ToonRenderer.h"
#include "ncengine/network/NetworkDispatcher.h"
#include "ncengine/physics/Collider.h"
#include "ncengine/physics/ConcaveCollider.h"
#include "ncengine/physics/PhysicsBody.h"

namespace nc::type
{
REGISTER_TYPE(Vector2, PROPERTY(Vector2, x), PROPERTY(Vector2, y));
REGISTER_TYPE(Vector3, PROPERTY(Vector3, x), PROPERTY(Vector3, y), PROPERTY(Vector3, z));
REGISTER_TYPE(Vector4, PROPERTY(Vector4, x), PROPERTY(Vector4, y), PROPERTY(Vector4, z), PROPERTY(Vector4, w));
REGISTER_TYPE(Quaternion, PROPERTY(Quaternion, x), PROPERTY(Quaternion, y), PROPERTY(Quaternion, z), PROPERTY(Quaternion, w));

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
REGISTER_TYPE(graphics::ToonRenderer);
REGISTER_TYPE(graphics::ParticleEmitter);
REGISTER_TYPE(graphics::PointLight);
REGISTER_TYPE(graphics::SkeletalAnimator);
REGISTER_TYPE(net::NetworkDispatcher);
REGISTER_TYPE(physics::ConcaveCollider);
REGISTER_TYPE(physics::Collider);
REGISTER_TYPE(physics::PhysicsBody);
} // namespace nc::type
