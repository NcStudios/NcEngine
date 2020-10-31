#pragma once

#include "UIElement.h"
#include "project/source/log/GameLog.h"
#include <deque>
#include <string>

namespace project
{
    class GameLog;
    
    namespace ui
    {
        class LogUIElement : public UIElement
        {
            public:
                LogUIElement(bool startOpen, GameLog* gameLog);
                void Draw() override;
            
            private:
                GameLog* m_gameLog;
        };
    }
}