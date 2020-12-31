#pragma once
#include "nc/source/component/Component.h"
#include "Physics.h"

#include <string>

namespace project
{
    class GamePiece : public nc::Component, public nc::physics::IClickable
    {
        public:
            GamePiece(nc::ComponentHandle handle, nc::EntityHandle parentHandle, nc::Transform* transform);
            ~GamePiece();
            void OnClick() override;

            void NetworkDispatchTest(uint8_t* data);
        
        private:
            bool m_isRegisteredAsClickable;
            std::string m_entityTag;
    };
}