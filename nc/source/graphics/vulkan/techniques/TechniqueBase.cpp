#include "TechniqueBase.h"
#include "graphics/vulkan/Base.h"
#include "graphics/vulkan/Swapchain.h"
#include "graphics/vulkan/FrameManager.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/Graphics2.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace nc::graphics::d3dresource;

namespace nc::graphics::vulkan
{
    TechniqueBase::TechniqueBase(TechniqueType techniqueType, vulkan::FrameManager* frameManager)
    : m_base{ GraphicsResourceManager::GetGraphics2()->GetBase() },
      m_swapchain{ GraphicsResourceManager::GetGraphics2()->GetSwapchain() },
      m_commands{ GraphicsResourceManager::GetGraphics2()->GetCommandsPtr() },
      m_frameManager{ frameManager },
      m_pipeline{},
      m_pipelineLayout{},
      m_descriptorSetLayout{},
      m_type{ techniqueType } 
    {
    }

    TechniqueBase::~TechniqueBase() noexcept
    {
        auto base = m_base.GetDevice();
        for (auto& pass : m_renderPasses)
        {
            base.destroyRenderPass(pass);
        }
        base.destroyDescriptorSetLayout(m_descriptorSetLayout);
        base.destroyPipelineLayout(m_pipelineLayout);
        base.destroyPipeline(m_pipeline);
    }

    TechniqueType TechniqueBase::GetType() const noexcept
    {
        return m_type;
    }
    
    vk::ShaderModule TechniqueBase::CreateShaderModule(const std::vector<uint32_t>& code, const vulkan::Base& base)
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.setCodeSize(code.size()*sizeof(uint32_t));
        createInfo.setPCode(code.data());
        vk::ShaderModule shaderModule;
        if (base.GetDevice().createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create shader module.");
        }

        return shaderModule;
    }

    std::vector<uint32_t> TechniqueBase::ReadShader(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open() || file.tellg() == -1)
        {
            throw std::runtime_error("Failed to open file.");
        }
        auto fileSize = static_cast<uint32_t>(file.tellg());
        if (fileSize % 4 != 0)
        {
            throw std::runtime_error("The file of shader byte code was not uint32_t aligned.");
        }

        auto bufferSize = fileSize/sizeof(uint32_t);
        std::vector<uint32_t> buffer(bufferSize);
        auto charBuffer = reinterpret_cast<char*>(buffer.data());
        file.seekg(0);
        file.read(charBuffer, fileSize);
        if (file.fail())
        {
            file.close();
            throw std::runtime_error("The file was wonky. (failbit set)");
        }
        file.close();
        return buffer;
    }
}