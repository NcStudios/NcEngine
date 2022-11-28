#include "ShaderUtilities.h"
#include "ncutility/NcError.h"

#include <fstream>

namespace nc::graphics
{
    vk::ShaderModule CreateShaderModule(vk::Device device, const std::vector<uint32_t>& code)
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.setCodeSize(code.size()*sizeof(uint32_t));
        createInfo.setPCode(code.data());
        vk::ShaderModule shaderModule;
        if (device.createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess)
        {
            throw NcError("Failed to create shader module.");
        }

        return shaderModule;
    }

    std::vector<uint32_t> ReadShader(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open() || file.tellg() == -1)
        {
            throw NcError("Failed to open file: ", filename);
        }
        auto fileSize = static_cast<uint32_t>(file.tellg());
        if (fileSize % 4 != 0)
        {
            throw NcError("The file of shader byte code was not uint32_t aligned: ", filename);
        }

        auto bufferSize = fileSize/sizeof(uint32_t);
        std::vector<uint32_t> buffer(bufferSize);
        auto charBuffer = reinterpret_cast<char*>(buffer.data());
        file.seekg(0);
        file.read(charBuffer, fileSize);
        if (file.fail())
        {
            file.close();
            throw NcError("The file was wonky. (failbit set)", filename);
        }
        file.close();
        return buffer;
    }
}