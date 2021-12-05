#pragma once

#include "ecs/component/AudioSource.h"
#include "ecs/component/AutoComponentGroup.h"
#include "ecs/component/Camera.h"
#include "ecs/component/Collider.h"
#include "ecs/component/ConcaveCollider.h"
#include "ecs/component/MeshRenderer.h"
#include "ecs/component/NetworkDispatcher.h"
#include "ecs/component/ParticleEmitter.h"
#include "ecs/component/PhysicsBody.h"
#include "ecs/component/PointLight.h"
#include "ecs/component/Tag.h"
#include "ecs/component/Transform.h"

namespace nc
{
    /** Should be replaced with Phil's type system once it is merged. */

    template<class T>
    struct TypeInfoBase
    {
        static constexpr auto name = "Null";
    };

    template<class T>
    struct TypeInfo : public TypeInfoBase<T> {};

    #define DEFINE_TYPE_INFO(Component)                                    \
    template<> struct TypeInfo<Component> : public TypeInfoBase<Component> \
    {                                                                      \
        static constexpr auto name = #Component;                           \
    };

    DEFINE_TYPE_INFO(AudioSource);
    DEFINE_TYPE_INFO(AutoComponentGroup);
    DEFINE_TYPE_INFO(Camera);
    DEFINE_TYPE_INFO(Collider);
    DEFINE_TYPE_INFO(ConcaveCollider);
    DEFINE_TYPE_INFO(Entity);
    DEFINE_TYPE_INFO(MeshRenderer);
    DEFINE_TYPE_INFO(NetworkDispatcher);
    DEFINE_TYPE_INFO(ParticleEmitter);
    DEFINE_TYPE_INFO(PhysicsBody);
    DEFINE_TYPE_INFO(PointLight);
    DEFINE_TYPE_INFO(Tag);
    DEFINE_TYPE_INFO(Transform);
}