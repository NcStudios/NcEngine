#pragma once

#include "shared/GameLog.h"

namespace nc::sample
{
    /** Simple collision event logging - requires a GameLog in the scene. */
    class CollisionLogger : public AutoComponent
    {
        public:
            CollisionLogger(Entity entity, Registry* registry);
            void OnCollisionEnter(Entity hit) override;
            void OnCollisionExit(Entity hit) override;
            void OnTriggerEnter(Entity hit) override;
            void OnTriggerExit(Entity hit) override;
        
        private:
            Registry* m_registry;
    };

    inline CollisionLogger::CollisionLogger(Entity entity, Registry* registry)
        : AutoComponent{entity},
          m_registry{registry}
    {
    }

    inline void CollisionLogger::OnCollisionEnter(Entity hit)
    {
        if(auto* tag = m_registry->Get<Tag>(hit); tag)
            GameLog::Log(std::string{"Collision Enter: "} + tag->Value().data());
    }

    inline void CollisionLogger::OnCollisionExit(Entity hit)
    {
        if(auto* tag = m_registry->Get<Tag>(hit); tag)
            GameLog::Log(std::string{"Collision Exit: "} + tag->Value().data());
    }

    inline void CollisionLogger::OnTriggerEnter(Entity hit)
    {
        if(auto* tag = m_registry->Get<Tag>(hit); tag)
            GameLog::Log(std::string{"Trigger Enter: "} + tag->Value().data());
    }

    inline void CollisionLogger::OnTriggerExit(Entity hit)
    {
        if(auto* tag = m_registry->Get<Tag>(hit); tag)
            GameLog::Log(std::string{"Trigger Exit: "} + tag->Value().data());
    }
}