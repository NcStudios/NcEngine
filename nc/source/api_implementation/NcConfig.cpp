#include "NcConfig.h"
#include "config/Config.h"

namespace nc::config
{
    const config::Config& NcGetConfigReference()
    {
        return Config::Get();
    }

    void NcSetUserName(std::string name)
    {
        Config::SetUserName(std::move(name));
    }
}