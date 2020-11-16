#pragma once

#include "ui/IUIElement.h"
#include "project/source/log/GameLog.h"
#include <deque>
#include <string>

namespace project
{
    namespace log { class GameLog; }
    
    namespace ui
    {
        class LogUIElement : public nc::ui::IUIElement
        {
            public:
                LogUIElement(bool startOpen, log::GameLog* gameLog);
                void Draw() override;
            
            private:
                log::GameLog* m_gameLog;
        };
    }
}