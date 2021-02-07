#pragma once

#include "Ecs.h"
#include "Physics.h"
#include "shared/GameLog.h"

#include <string>

namespace nc::sample
{
    class Clickable : public Component, public physics::IClickable
    {
        public:
            Clickable(EntityHandle handle, physics::LayerMask mask);
            ~Clickable();
            void OnClick() override;
        
        private:
            std::string m_Tag;
    };

    inline Clickable::Clickable(EntityHandle handle, physics::LayerMask mask)
        : Component(handle),
          physics::IClickable(GetComponent<Transform>(handle), 40.0f),
          m_Tag{GetEntity(handle)->Tag}
    {
        physics::RegisterClickable(this);
        IClickable::layers = mask;
    }

    inline Clickable::~Clickable()
    {
        physics::UnregisterClickable(this);
    }

    inline void Clickable::OnClick()
    {
        GameLog::Log("Clicked: " + m_Tag);
    }
}