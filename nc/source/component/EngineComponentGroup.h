#pragma once
#include "NcCommonTypes.h"

namespace nc
{
    /** 
     * Collection of engine component
     * handles held by entities. 
     */
    struct EngineComponentHandleGroup
    {
        ComponentHandle transform = NullHandle;
        ComponentHandle renderer = NullHandle;
        ComponentHandle pointLight = NullHandle;
        ComponentHandle networkDispatcher = NullHandle;
    };
} //end namespace::nc