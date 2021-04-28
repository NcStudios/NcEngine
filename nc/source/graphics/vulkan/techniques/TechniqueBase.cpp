#include "TechniqueBase.h"
#include "component/Transform.h"
#include "graphics/vulkan/Base.h"
#include "graphics/vulkan/Swapchain.h"
#include "graphics/vulkan/TechniqueManager.h"
#include "graphics/Graphics2.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

namespace nc::graphics::vulkan
{
    TechniqueBase::TechniqueBase(TechniqueType techniqueType, const GlobalData& globalData, nc::graphics::Graphics2* graphics)
    : m_graphics{ graphics },
      m_base{ graphics->GetBase() },
      m_swapchain{ graphics->GetSwapchain() },
      m_globalData{ globalData },
      m_meshes{},
      m_objects{},
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

    void TechniqueBase::RegisterRenderer(Mesh mesh, Transform* transform)
    {
        for (auto& registeredMesh : m_meshes)
        {
            if (registeredMesh.uid.compare(mesh.uid) == 0)
            {
                auto& objects = m_objects.at(registeredMesh.uid);
                objects.push_back(transform);
                return;
            }
        }
        m_meshes.push_back(mesh);
        m_objects.emplace(mesh.uid, std::vector<Transform*>{transform} );
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