#pragma once

#include "Type.h"
#include "ecs/component/All.h"

namespace nc
{
    REGISTER_TYPE(Vector2, PROPERTY(Vector2, x), PROPERTY(Vector2, y));
    REGISTER_TYPE(Vector3, PROPERTY(Vector3, x), PROPERTY(Vector3, y), PROPERTY(Vector3, z));
    REGISTER_TYPE(Vector4, PROPERTY(Vector4, x), PROPERTY(Vector4, y), PROPERTY(Vector4, z), PROPERTY(Vector4, w));
    REGISTER_TYPE(Quaternion, PROPERTY(Quaternion, x), PROPERTY(Quaternion, y), PROPERTY(Quaternion, z), PROPERTY(Quaternion, w));
    REGISTER_TYPE(PointLightInfo, PROPERTY(PointLightInfo, ambient), PROPERTY(PointLightInfo, diffuseColor), PROPERTY(PointLightInfo, diffuseIntensity));

    /** @todo Only registering names of these: */
    REGISTER_TYPE(AudioSource);
    REGISTER_TYPE(AttachmentGroup);
    REGISTER_TYPE(Camera);
    REGISTER_TYPE(Collider);
    REGISTER_TYPE(ConcaveCollider);
    REGISTER_TYPE(Entity);
    REGISTER_TYPE(MeshRenderer);
    REGISTER_TYPE(NetworkDispatcher);
    REGISTER_TYPE(ParticleEmitter);
    REGISTER_TYPE(PhysicsBody);
    REGISTER_TYPE(PointLight);
    REGISTER_TYPE(Tag);
    REGISTER_TYPE(Transform);
}