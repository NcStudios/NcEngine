#pragma once

#include "component/Component.h"
#include "Entity.h"
#include "source/log/GameLog.h"

namespace project
{
    /** Simple collision event loggin - requires a GameLog in the scene. */
    class CollisionLogger : public nc::Component
    {
        public:
            CollisionLogger(nc::EntityHandle parentHandle)
                : Component{parentHandle}
            {
            }

            void OnCollisionEnter(nc::Entity* hit) override
            {
                log::GameLog::Log("Collision Enter: " + hit->Tag);
            }

            void OnCollisionStay(nc::Entity* hit) override
            {
                log::GameLog::Log("Collision Stay: " + hit->Tag);
            }

            void OnCollisionExit(nc::Entity* hit) override
            {
                log::GameLog::Log("Collision Exit: " + hit->Tag);
            }
    };
}