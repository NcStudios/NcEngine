#pragma once

#include "graphics/PerFrameRenderState.h"

namespace nc::graphics
{
    class ITechnique
    {
        public:
            virtual ~ITechnique() = default;

            virtual bool CanBind(const PerFrameRenderState& frameData) = 0;
            virtual void Bind(vk::CommandBuffer* cmd) = 0;

            virtual bool CanRecord(const PerFrameRenderState& frameData) = 0;
            virtual void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) = 0;

            auto GetUid() ->const std::string& { return m_uid; }

        private:
            std::string m_uid;
    };
}