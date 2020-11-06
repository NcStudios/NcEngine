#pragma once

#include "log/IGamelog.h"
#include <string>

namespace nc::log
{
    void NcRegisterGameLog(log::IGameLog* gameLog);

    void NcLogToGame(std::string item);

    void NcLogToDiagnostics(std::string item);
}