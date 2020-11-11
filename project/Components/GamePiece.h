#pragma once
#include "component/Component.h"
#include "NcPhysics.h"

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