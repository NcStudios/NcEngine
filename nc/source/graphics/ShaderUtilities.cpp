#include "ShaderUtilities.h"
#include "graphics/Base.h"

#include <fstream>

namespace nc::graphics
{
    vk::ShaderModule CreateShaderModule(const std::vector<uint32_t>& code, Base* base)
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.setCodeSize(code.size()*sizeof(uint32_t));
        createInfo.setPCode(code.data());
        vk::ShaderModule shaderModule;
        if (base->GetDevice().createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create shader module.");
        }

        return shaderModule;
    }

    std::vector<uint32_t> ReadShader(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open() || file.tellg() == -1)
        {
            throw std::runtime_error("Failed to open shader file: " + filename);
        }
        auto fileSize = static_cast<uint32_t>(file.tellg());
        if (fileSize % 4 != 0)
        {
            throw std::runtime_error("Shader byte code was not uint32_t aligned: " + filename);
        }

        auto bufferSize = fileSize/sizeof(uint32_t);
        std::vector<uint32_t> buffer(bufferSize);
        auto charBuffer = reinterpret_cast<char*>(buffer.data());
        file.seekg(0);
        file.read(charBuffer, fileSize);
        if (file.fail())
        {
            file.close();
            throw std::runtime_error("Failure reading shader: " + filename);
        }
        file.close();
        return buffer;
    }
}