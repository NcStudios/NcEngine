#pragma once

#include "ncutility/NcError.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/Shader.h"

#include <span>
#include <string_view>
#include <vector>

namespace nc::graphics
{
auto ReadShaderFile(std::string_view filePath) -> std::vector<char>;

class ShaderFactory
{
    public:
        explicit ShaderFactory(Diligent::IRenderDevice& device,
                               Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> streamFactory,
                               bool isRowMajor)
            : m_device{&device},
              m_streamFactory{std::move(streamFactory)},
              m_isRowMajor{isRowMajor}
        {
        }

        auto HasRuntimeCompilationSupport() const -> bool
        {
            return m_streamFactory;
        }

        auto MakeShaderFromSource(std::span<const char> source,
                                  std::string_view name,
                                  Diligent::SHADER_TYPE type,
                                  Diligent::SHADER_SOURCE_LANGUAGE language = Diligent::SHADER_SOURCE_LANGUAGE_HLSL,
                                  std::string_view entryPoint = "main") -> Diligent::RefCntAutoPtr<Diligent::IShader>;

    private:
        Diligent::IRenderDevice* m_device;
        Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory> m_streamFactory;
        bool m_isRowMajor;
};

template<class EngineFactoryT>
static auto MakeShaderFactory([[maybe_unused]] EngineFactoryT& engineFactory,
                              Diligent::IRenderDevice& device,
                              bool isRowMajor) -> std::unique_ptr<ShaderFactory>
{
    auto shaderSourceFactory = Diligent::RefCntAutoPtr<Diligent::IShaderSourceInputStreamFactory>{};
#if NC_RUNTIME_SHADER_COMPILATION
    engineFactory.CreateDefaultShaderSourceStreamFactory(nullptr, &shaderSourceFactory);
    if (!shaderSourceFactory)
    {
        throw NcError{"Failed to create ShaderSourceInputStreamFactory"};
    }
#endif
    return std::make_unique<ShaderFactory>(device, std::move(shaderSourceFactory), isRowMajor);
}
} // namespace nc::graphics
