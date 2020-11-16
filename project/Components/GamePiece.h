#pragma once
#include "component/Component.h"
#include "Physics.h"

#include <string>

namespace project
{
    class GamePiece : public nc::Component, public nc::physics::IClickable
    {
        public:
            GamePiece(nc::Transform* transform);
            ~GamePiece();
            void OnClick() override;
        
        private:
            bool m_isRegisteredAsClickable;
            std::string m_entityTag;
    };
}