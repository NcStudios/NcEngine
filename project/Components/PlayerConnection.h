#pragma once

#include "component/Component.h"
#include "source/network/client/Client.h"

#include <string>

namespace project
{
    class PlayerConnection : public nc::Component
    {
        public:
            PlayerConnection(std::string playerName);
            void FrameUpdate(float dt) override;

        private:
            network::Client m_client;
            std::string m_playerName;
    };
}