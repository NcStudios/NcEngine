#pragma once

#include "entity/Entity.h"
#include "shared/GameLog.h"

namespace nc::sample
{
    /** Simple collision event logging - requires a GameLog in the scene. */
    class CollisionLogger : public Component
    {
        public:
            CollisionLogger(EntityHandle parentHandle);
            void OnCollisionEnter(EntityHandle hit) override;
            void OnCollisionStay(EntityHandle hit) override;
            void OnCollisionExit(EntityHandle hit) override;
    };

    inline CollisionLogger::CollisionLogger(EntityHandle parentHandle)
        : Component{parentHandle}
    {
    }

    inline void CollisionLogger::OnCollisionEnter(EntityHandle hit)
    {
        if(auto* other = GetEntity(hit); other)
            GameLog::Log("Collision Enter: " + other->Tag);
    }

    inline void CollisionLogger::OnCollisionStay(EntityHandle hit)
    {
        if(auto* other = GetEntity(hit); other)
            GameLog::Log("Collision Stay: " + other->Tag);
    }

    inline void CollisionLogger::OnCollisionExit(EntityHandle hit)
    {
        if(auto* other = GetEntity(hit); other)
            GameLog::Log("Collision Exit: " + other->Tag);
    }
}