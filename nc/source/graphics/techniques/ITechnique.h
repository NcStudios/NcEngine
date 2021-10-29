#pragma once

#include "graphics/PerFrameRenderState.h"

namespace nc::graphics
{
    class ITechnique
    {
        public:
            virtual ~ITechnique() = default;

            virtual bool CanBind(PerFrameRenderState* frameData) = 0;
            virtual void Bind(vk::CommandBuffer* cmd) = 0;

            virtual bool CanRecord(PerFrameRenderState* frameData) = 0;
            virtual void Record(vk::CommandBuffer* cmd, PerFrameRenderState* frameData) = 0;
    };
}