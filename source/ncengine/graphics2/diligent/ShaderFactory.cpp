#include "ShaderFactory.h"

#include <filesystem>
#include <fstream>

namespace nc::graphics
{
auto ShaderFactory::MakeShaderFromSource(std::span<const char> source,
                                         std::string_view name,
                                         Diligent::SHADER_TYPE type,
                                         Diligent::SHADER_SOURCE_LANGUAGE language,
                                         std::string_view entryPoint) -> Diligent::RefCntAutoPtr<Diligent::IShader>
{
    if (!HasRuntimeCompilationSupport())
    {
        throw NcError{"Runtime shader compilation not enabled"};
    }

    auto createInfo = Diligent::ShaderCreateInfo{};
    createInfo.Source = source.data();
    createInfo.SourceLength = source.size();
    createInfo.pShaderSourceStreamFactory = m_streamFactory;
    createInfo.EntryPoint = entryPoint.data();
    createInfo.Desc.Name = name.data();
    createInfo.Desc.ShaderType = type;
    createInfo.Desc.UseCombinedTextureSamplers = true;
    createInfo.SourceLanguage = language;
    createInfo.CompileFlags = Diligent::SHADER_COMPILE_FLAG_PACK_MATRIX_ROW_MAJOR |
                              Diligent::SHADER_COMPILE_FLAG_ENABLE_UNBOUNDED_ARRAYS;
    return CreateShader(createInfo);
}

auto ShaderFactory::MakeShaderFromByteCode(std::span<const char> byteCode,
                                           std::string_view name,
                                           Diligent::SHADER_TYPE type) -> Diligent::RefCntAutoPtr<Diligent::IShader>
{
    auto createInfo = Diligent::ShaderCreateInfo{};
    createInfo.ByteCode = byteCode.data();
    createInfo.ByteCodeSize = byteCode.size();
    createInfo.Desc.Name = name.data();
    createInfo.Desc.ShaderType = type;
    createInfo.Desc.UseCombinedTextureSamplers = true;
    return CreateShader(createInfo);
}

auto ShaderFactory::ReadShaderFile(std::string_view filePath) -> std::vector<char>
{
    auto shaderPath = (std::filesystem::path(m_shadersPath) / filePath).string();
    auto file = std::ifstream(shaderPath, std::ios::ate | std::ios::binary);
    if (!file.is_open() || file.tellg() == -1)
    {
        throw NcError(fmt::format("Failed to open shader file '{}'",shaderPath));
    }

    const auto fileSize = static_cast<uint32_t>(file.tellg());
    auto buffer = std::vector<char>(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    if (file.fail())
    {
        throw NcError(fmt::format("Failed to read shader file '{}'", shaderPath));
    }

    return buffer;
}

auto ShaderFactory::CreateShader(const Diligent::ShaderCreateInfo& createInfo) -> Diligent::RefCntAutoPtr<Diligent::IShader>
{
    auto shader = Diligent::RefCntAutoPtr<Diligent::IShader>{};
    m_device->CreateShader(createInfo, &shader);
    if (!shader)
    {
        throw NcError(fmt::format("Failed to create shader '{}'", createInfo.Desc.Name));
    }

    return shader;
}
} // namespace nc::graphics
