#pragma once
#include "config/Config.h"

namespace nc
{
    namespace config
    {        
        /**
         * Returns a reference to the Config data object.
         * @return Const reference to Config on success.
         */
        const config::Config& NcGetConfigReference();
    }
}