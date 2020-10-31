#pragma once

#include "log/Ilog.h"
#include <string>

namespace nc::log
{
    void NcRegisterGameLog(log::ILog* gameLog);

    void NcLogToGame(std::string item);
}