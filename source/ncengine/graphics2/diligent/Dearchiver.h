#pragma once

#include "Common/interface/RefCntAutoPtr.hpp"

#include "ncengine/type/StableAddress.h"

#include <string_view>

namespace Diligent
{
struct IDearchiver;
struct IEngineFactory;
struct IRenderDevice;
struct ISwapChain;
struct IPipelineResourceSignature;
struct IPipelineState;
struct PipelineStateCreateInfo;
struct PipelineStateUnpackInfo;
} // namespace Diligent

namespace nc::graphics
{
class Dearchiver : public StableAddress
{
    public:
        explicit Dearchiver(Diligent::IRenderDevice& device,
                            Diligent::ISwapChain& swapChain,
                            Diligent::IEngineFactory& factory);

        void Load(std::string_view archivePath);
        auto UnpackResourceSignature(std::string_view name) -> Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature>;
        auto UnpackGraphicsPipelineState(std::string_view name) -> Diligent::RefCntAutoPtr<Diligent::IPipelineState>;
        void Clear();

    private:
        Diligent::IRenderDevice* m_device;
        Diligent::ISwapChain* m_swapChain;
        Diligent::RefCntAutoPtr<Diligent::IDearchiver> m_dearchiver;

        auto UnpackPipelineStateImpl(const Diligent::PipelineStateUnpackInfo& info) -> Diligent::RefCntAutoPtr<Diligent::IPipelineState>;
};
} // namespace nc::graphics
