#include "Dearchiver.h"

#include "ncutility/NcError.h"

#include "Common/interface/DataBlobImpl.hpp"
#include "Graphics/GraphicsEngine/interface/Dearchiver.h"
#include "Graphics/GraphicsEngine/interface/EngineFactory.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"
#include "Graphics/GraphicsEngine/interface/PipelineState.h"

#include <fstream>

namespace
{
void UnpackGraphicsPipelineCB(Diligent::PipelineStateCreateInfo& ci, void* userData)
{
    NC_ASSERT(ci.PSODesc.PipelineType == Diligent::PIPELINE_TYPE_GRAPHICS, "Pipeline type mismatch");
    auto& upcastCI = static_cast<Diligent::GraphicsPipelineStateCreateInfo&>(ci);
    auto& graphicsPipeline = upcastCI.GraphicsPipeline;
    auto* swapChain = static_cast<Diligent::ISwapChain*>(userData);
    graphicsPipeline.NumRenderTargets = 1;
    graphicsPipeline.RTVFormats[0] = swapChain->GetDesc().ColorBufferFormat;
    graphicsPipeline.DSVFormat = swapChain->GetDesc().DepthBufferFormat;
}
} // anonymous namespace

namespace nc::graphics
{
Dearchiver::Dearchiver(Diligent::IRenderDevice& device,
                       Diligent::ISwapChain& swapChain,
                       Diligent::IEngineFactory& factory)
    : m_device{&device},
      m_swapChain{&swapChain}
{
    auto dearchiverCI = Diligent::DearchiverCreateInfo{};
    factory.CreateDearchiver(dearchiverCI, &m_dearchiver);
    if (!m_dearchiver)
    {
        throw NcError("Failed to create Dearchiver.");
    }
}

void Dearchiver::Load(std::string_view archivePath)
{
    auto file = std::ifstream{archivePath.data(), std::ios::ate | std::ios::binary};
    if (!file)
    {
        throw NcError{fmt::format("Failed to open archive '{}'", archivePath)};
    }

    const auto fileSize = static_cast<size_t>(file.tellg());
    auto blob = Diligent::DataBlobImpl::Create(fileSize);
    file.seekg(0);
    file.read(static_cast<char*>(blob->GetDataPtr()), fileSize);
    if (file.fail() || !m_dearchiver->LoadArchive(blob))
    {
        throw NcError{fmt::format("Failed to load archive '{}'", archivePath)};
    }
}

auto Dearchiver::UnpackResourceSignature(std::string_view name) -> Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature>
{
    const auto info = Diligent::ResourceSignatureUnpackInfo{
        .pDevice = m_device,
        .Name = name.data()
    };

    auto signature = Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature>{};
    m_dearchiver->UnpackResourceSignature(info, &signature);
    if (!signature)
    {
        throw NcError{fmt::format("Failed to unpack resource signature '{}'", name)};
    }

    return signature;
}

auto Dearchiver::UnpackGraphicsPipelineState(std::string_view name) -> Diligent::RefCntAutoPtr<Diligent::IPipelineState>
{
    const auto info = Diligent::PipelineStateUnpackInfo{
        .pDevice = m_device,
        .Name = name.data(),
        .PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS,
        .ModifyPipelineStateCreateInfo = &::UnpackGraphicsPipelineCB,
        .pUserData = static_cast<void*>(m_swapChain)
    };

    return UnpackPipelineStateImpl(info);
}

void Dearchiver::Clear()
{
    m_dearchiver->Reset();
}

auto Dearchiver::UnpackPipelineStateImpl(const Diligent::PipelineStateUnpackInfo& info) -> Diligent::RefCntAutoPtr<Diligent::IPipelineState>
{
    auto pipelineState = Diligent::RefCntAutoPtr<Diligent::IPipelineState>{};
    m_dearchiver->UnpackPipelineState(info, &pipelineState);
    if (!pipelineState)
    {
        throw NcError{fmt::format("Failed to unpack pipeline state '{}'", info.Name)};
    }

    return pipelineState;
}
} // namespace nc::graphics
