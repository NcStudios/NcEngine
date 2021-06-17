#pragma once

#include "Ecs.h"
#include "Physics.h"
#include "Entity.h"
#include "shared/GameLog.h"

#include <string>

namespace nc::sample
{
    class Clickable : public AutoComponent, public physics::IClickable
    {
        public:
            Clickable(Entity entity, std::string tag);
            ~Clickable() noexcept;
            void OnClick() override;
        
        private:
            std::string m_Tag;
    };

    inline Clickable::Clickable(Entity entity, std::string tag)
        : AutoComponent(entity),
          physics::IClickable(entity, 40.0f),
          m_Tag{std::move(tag)}
    {
        physics::RegisterClickable(this);
        auto layer = EntityUtils::Layer(entity);
        IClickable::layers = physics::ToLayerMask(layer);
    }

    inline Clickable::~Clickable() noexcept
    {
        physics::UnregisterClickable(this);
    }

    inline void Clickable::OnClick()
    {
        GameLog::Log("Clicked: " + m_Tag);
    }
}