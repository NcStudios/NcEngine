#pragma once

#include "Ecs.h"
#include "physics/PhysicsSystem.h"
#include "Entity.h"
#include "shared/GameLog.h"

#include <string>

namespace nc::sample
{
    class Clickable : public AutoComponent, public IClickable
    {
        public:
            Clickable(Entity entity, std::string tag, PhysicsSystem* physicsSystem);
            ~Clickable() noexcept;
            void OnClick() override;
        
        private:
            std::string m_Tag;
            PhysicsSystem* m_physicsSystem;
    };

    inline Clickable::Clickable(Entity entity, std::string tag, PhysicsSystem* physicsSystem)
        : AutoComponent(entity),
          IClickable(entity, 40.0f),
          m_Tag{std::move(tag)},
          m_physicsSystem{physicsSystem}

    {
        m_physicsSystem->RegisterClickable(this);
        auto layer = entity.Layer();
        IClickable::layers = ToLayerMask(layer);
    }

    inline Clickable::~Clickable() noexcept
    {
        m_physicsSystem->UnregisterClickable(this);
    }

    inline void Clickable::OnClick()
    {
        GameLog::Log("Clicked: " + m_Tag);
    }
}