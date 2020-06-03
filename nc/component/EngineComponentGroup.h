#pragma once
#include "Common.h"

namespace nc
{
    /** 
     * Collection of engine component
     * handles held by entities. 
     */
    struct EngineComponentHandleGroup
    {
        ComponentHandle transform =  NullHandle;
        ComponentHandle renderer  =  NullHandle;
        ComponentHandle pointLight = NullHandle;
    };
} //end namespace::nc