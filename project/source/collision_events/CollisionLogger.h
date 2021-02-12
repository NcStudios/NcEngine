#pragma once

#include "Entity.h"
#include "shared/GameLog.h"

namespace nc::sample
{
    /** Simple collision event logging - requires a GameLog in the scene. */
    class CollisionLogger : public Component
    {
        public:
            CollisionLogger(EntityHandle parentHandle);
            void OnCollisionEnter(Entity* hit) override;
            void OnCollisionStay(Entity* hit) override;
            void OnCollisionExit(Entity* hit) override;
    };

    inline CollisionLogger::CollisionLogger(EntityHandle parentHandle)
        : Component{parentHandle}
    {
    }

    inline void CollisionLogger::OnCollisionEnter(Entity* hit)
    {
        GameLog::Log("Collision Enter: " + hit->Tag);
    }

    inline void CollisionLogger::OnCollisionStay(Entity* hit)
    {
        GameLog::Log("Collision Stay: " + hit->Tag);
    }

    inline void CollisionLogger::OnCollisionExit(Entity* hit)
    {
        GameLog::Log("Collision Exit: " + hit->Tag);
    }
}