#include "GamePiece.h"
#include "NcLog.h"

namespace project
{
    GamePiece::GamePiece(nc::Transform* transform)
        : nc::physics::IClickable(transform, 20.0f),
          m_isRegisteredAsClickable{ true },
          m_entityTag{}
    {
        nc::physics::NcRegisterClickable(this);
        layers |= nc::physics::Layer::GamePiece;
    }

    GamePiece::~GamePiece()
    {
        if(m_isRegisteredAsClickable)
        {
            nc::physics::NcUnregisterClickable(this);
        }
    }

    void GamePiece::OnClick()
    {
        if(m_entityTag == "")
        {
            m_entityTag = nc::NcGetEntity(m_parentHandle)->Tag;
        }
        nc::log::NcLogToGame("Clicked: " + m_entityTag);
    }
}